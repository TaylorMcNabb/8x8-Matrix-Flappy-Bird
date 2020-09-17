/*
 * Name: Taylor McNabb
 * Date: June 7, 2019
 * Purpose: Creates a Flappy Bird game using two matrix displays.
 */

// Libraries
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include </usr/include/sys/io.h>

// Define the two parallel port outputs
#define row 0x378
#define rowSelect 0x37A

// Function declaration
void display();
void flapBird();
void movePillar();
void checkCollision();
void endGame();

// Variable declaration
int currentPillar;
int currentPanel = 2;
int pillars[5][4] = {{0, 1, 2, 3}, {0, 1, 2, 7}, {0, 1, 6, 7}, {0, 5, 6, 7}, {4, 5, 6, 7}};
int pillarX = 128;
int birdY = 1;
int score = 0;
int gameOver = 0;
int flap;
char input;

// Main function
int main() {
	// Seeds the random number generator and selects a random pillar
	srand(time(NULL));
	currentPillar = rand() % 5;
	// Initializes the parallel port and the curses window
	ioperm(row, 3, 1);
	initscr();
	timeout(0);
	cbreak();
	noecho();
	while (1) {
		// This loop displays the game on the matrix displays
		for (int i = 0; i < 10; i++) {
			input = getch();	// Gets the key that the user is pressing
			// Sets the flap variable to 3 to indicate that the bird should move up
			if (input == 'w') {
				flap = 3;
			}
			// Ends the program if the 'x' key was pressed
			else if (input == 'x') {
				outb(255, row);
				ioperm(row, 3, 0);
				endwin();
				return 0;
			}
			// Calls the display() function to display the game on the matrix displays
			display();
		}
		// Moves the bird and pillar, and checks if the bird has collided with the pillar
		flapBird();
		movePillar();
		checkCollision();
		// Ends the game if the gameOver variable is set to 1
		if (gameOver == 1) {
			return 0;
		}
	}
}

// This function moves the bird
void flapBird() {
	// Moves the bird up based on the value of flap
	if (flap != 0) {
		birdY--;
		flap--;
	}
	// Moves the bird down if flap equals zero
	else {
		birdY++;
	}
	// Ends the game if the user moved the bird off the display
	if (birdY < 0 || birdY > 7) {
		endGame();
	}
}

// This function moves the pillar
void movePillar() {
	// Moves the pillar to the left
	pillarX = pillarX / 2;
	// If the pillar is crossing to the first display, the currentPanel variable is reduced
	if (pillarX == 0) {
		pillarX = 128;
		currentPanel--;
	}
	// If the pillar reaches the end of the first display, the pillar is reset and moved to the second display
	// The score is also incremented and a beep sound is played
	if (currentPanel == 0) {
		score++;
		beep();
		currentPillar = rand() % 5;
		currentPanel = 2;
	}
}

// This function displays the game on the matrix displays
void display() {
	// Prints the instructions and the current score to the window
	mvprintw(10, 31, "Press 'w' to flap!");
	mvprintw(11, 28, "Your current score is %d.", score);
	refresh();
	// Displays the bird on the matrix display
	outb(birdY ^ 0xB, rowSelect);
	outb(2 ^ 0xFF, row);
	usleep(2000);
	if (currentPanel == 1) {
		// This loop displays the pillar if it is on the first matrix display
		for (int i = 0; i < 4; i++) {
			outb(pillars[currentPillar][i] ^ 0xB, rowSelect);
			outb(pillarX ^ 0xFF, row);
			usleep(2000);
		}
	}
	else {
		// This loop displays the pillar if it is on the second matrix display
		for (int i = 0; i < 4; i++) {
			outb((pillars[currentPillar][i] + 8) ^ 0xB, rowSelect);
			outb(pillarX ^ 0xFF, row);
			usleep(2000);
		}
	}
}

// This function checks if the bird has collided with the pillar
void checkCollision() {
	// Checks for collision if the pillar is at the same X coordinate as the bird
	if (currentPanel == 1 && pillarX == 2) {
		// This loop checks if the bird has collided with the pillar
		for (int i = 0; i < 4; i++) {
			if (birdY == pillars[currentPillar][i]) {
				endGame();	// Ends the game if a collision has occured
			}
		}
	}
}

// This function ends the program
void endGame() {
	outb(255, row);
	// Clears the window and outputs the user's final score
	erase();
	mvprintw(10, 35, "You lost!");
	mvprintw(11, 28, "Your final score was %d.", score);
	mvprintw(12, 29, "Press any key to exit.");
	refresh();
	// Waits for the user to press a key
	getchar();
	// Ends the program
	ioperm(row, 3, 0);
	endwin();
	gameOver = 1;
}
