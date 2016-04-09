/*****************************************************
** Author: Clare Brown
** Date: 3/4/2016
** Description: Board class declaration file. Contains
** enum States to hold states of board, 3x3 character
** array member variable. Declares default constructor,
** makeMove function, gameState function, and print function.
******************************************************/

#ifndef BOARD_HPP
#define BOARD_HPP

enum State { X, O, DRAW, PROGRESS};

class Board { 
	//member variables
private:
	char bArray[3][3];

	//member functions
public:
	Board();
	bool makeMove(int, int, char);
	State gameState();
	void print();
};

#endif // !BOARD_HPP
