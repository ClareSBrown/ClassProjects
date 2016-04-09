/*****************************************************
** Author: Clare Brown
** Date: 3/4/2016
** Description: Board class implementation file. Defines 
** a board for a tic tac toe with a constructor, a makeMove 
** function, a gameState function, and a print function. 
******************************************************/
#include "Board.hpp"
#include <iostream>


/*****************************************************
** Description: Default constructor for Board. Initializes
** the array of characters with '.' character in each
** position.
******************************************************/
Board::Board() {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			bArray[i][j] = '.';
		}
	}
}
/*****************************************************
** Description: makeMove function. Takes two ints that
** represent the row and column of the array and a
** char which represents which player is making the move.
** Function checks to see if the row and column are valid
** and if the space in the board array is empty. If so, 
** it places the character of the player in the specified 
** space. If not, does not change any locations in the array.
** Returns true if move is made, returns false otherwise.
******************************************************/
bool Board::makeMove(int row, int col, char player) {
	if (row >= 0 && row < 3 && col >= 0 && col < 3) {
		if (bArray[row][col] == '.') {
			bArray[row][col] = player; //placing player character in location
			return true;
		}
	}
	return false;
}


/*******************************************************
Description: gameState function. Tests whether or not
** any player has won horizontally, vertically, or 
** diagonally or if there is a draw. Returns the appropriate
** State depending on the state of the board.
********************************************************/
State Board::gameState() {
	char player = '.';

	//testing horizontal win
	for (int i = 0; i < 3; i++) { //looping through rows
		player = bArray[i][0]; //getting character in first column of row to test against
		//if the space is not "empty" and all three in row are same, return player
		if (player != '.' && player == bArray[i][1] && player == bArray[i][2]) {
			if (player == 'X')
				return X;
			else
				return O;
		}
	}

	//testing vertical win
	for (int j = 0; j < 3; j++) { //looping through columns
		player = bArray[0][j]; //getting character in first row of column to test against
		if (player != '.' && player == bArray[1][j] && player == bArray[2][j]) {
			if (player == 'X')
				return X;
			else
				return O;
		}
	}

	//testing diagonal wins
	//top left to bottom right diagonal
	player = bArray[0][0]; //getting character in top left to test against
	if (player !='.' && player == bArray[1][1] && player == bArray[2][2]) { 
		if (player == 'X')
			return X;
		else
			return O;
		}
	//top right to bottom left diagonal
	player = bArray[0][2]; //getting character in top right to test against
	if (player != '.' && player == bArray[1][1] && player == bArray[2][0]) {
		if (player == 'X')
			return X;
		else
			return O;
	}

	// searching for an empty spot to see if the game is still in progress
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (bArray[i][j] == '.') {//if any spot is "empty" game is still in progress
				return PROGRESS;
			}
		}
	}
	return DRAW; 
}


/***********************************************************
** Description: print function. Prints a representation of the
** data that is stored in bArray. Puts row and column numbers
** to describe positions.
************************************************************/
void Board::print() {
	std::cout << "  0 1 2" << std::endl; //column numbers
	for (int i = 0; i < 3; i++) {
		std::cout << i << " "; //printing row number
		for (int j = 0; j < 3; j++) {
			std::cout << bArray[i][j] << " "; //printing what is in the locations
		}
		std::cout << std::endl;		
	}
	std::cout << std::endl;
}