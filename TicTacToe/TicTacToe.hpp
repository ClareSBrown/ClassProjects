/******************************************************
** Author: Clare Brown
** Date 3/4/2016
** Description: TicTacToe class declaration file. Contains
** Board and char member variables to represent the board
** that the tic tac toe game is being played on and the 
** player that is currently playing the game. Declares
** constructor which takes a char and a play function
** which will trigger the game to start.
*******************************************************/
#ifndef TICTACTOE_HPP
#define TICTACTOE_HPP
#include "Board.hpp"

class TicTacToe {
private:
	Board game;
	char player;
public:
	TicTacToe();
	TicTacToe(char);
	void play();
};
#endif // !TICTACTOE_HPP
