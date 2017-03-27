/*******************************************************
** Author: Clare Brown
** Date: 3/4/2016
** Description: TicTacToe class implementation file. Uses
** the board class to simulate the playing of a tic tac 
** toe game with two players. Defines the constructor and
** play function. Also contains a main function that uses
** functions to play a tic tac toe game.
********************************************************/
#include "TicTacToe.hpp"
#include <iostream>
//#include <limits>


/*******************************************************
** Description: Default constructor. Sets the player to 
** 'X'
********************************************************/
TicTacToe::TicTacToe() {
	this->player = 'X';
}


/*******************************************************
** Description: Constructor that takes a character as
** a parameter and sets that character to the player 
** member variable.
********************************************************/
TicTacToe::TicTacToe(char player) {
	this->player = player;
}


/*******************************************************
** Description: play function. Uses member variables to
** simulate a tic tac toe game. Proceeds while gameState
** is PROGRESS and ends when one player wins or there
** is a draw. Switches player every round unless a player
** enters the wrong input. 
********************************************************/
void TicTacToe::play() {
	int row, col;
	bool validMove;
	while (game.gameState() == PROGRESS) {
		game.print(); //printing board so it can be seen
		std::cout << "Player " << player << ": Please enter your move, row and then column separated by a space.";
		std::cout << std::endl;
		std::cin >> row >> col; //taking player input
		std::cout << std::endl;
		validMove = game.makeMove(row, col, player); //testing if the move is allowed
		if (validMove) {
			if (player == 'X')
				player = 'O';
			else
				player = 'X';
		}
		else {
			std::cout << "That square is already taken or the input is invalid." << std::endl;//if move is not allowed
			std::cin.clear();//preventing infinite loop if inputs are not ints
			std::cin.ignore(20,'\n');
		}
		
			
	}
	game.print(); //printing board at end of game
	//Determining results of game
	if (game.gameState() == X)
		std::cout << "X Won!" << std::endl;
	else if (game.gameState() == O)
		std::cout << "O Won!" << std::endl;
	else
		std::cout << "Draw!" << std::endl;
}


/*******************************************************
** Description: Main function. Prompts user to enter what
** player will go first, verifies the entry, and then
** begins the Tic Tac Toe game.
********************************************************/
int main() {
	char playerIn;
	std::cout << "Please enter which player will be going first: X or O" << std::endl;
	std::cin >> playerIn;
	while (playerIn != 'O' && playerIn != 'X') {
		std::cout << "Incorrect input. Please enter X or O." << std::endl;
		std::cin >> playerIn;
	}
	TicTacToe tttgame(playerIn);
	
	tttgame.play();
	return 0;
}