/**********************************************************
** Author: Clare Brown
** Date: 3/4/2016
** Description: Book class declaration file. Class
** inherits from the LibraryItem class. Contains a member
** variable that is a string to hold the author of the Book.
** Declares a constructor with 3 arguments, a getAuthor()
** function and an override of the pure virtual
** getCheckOutLength() function from LibraryItem. Also
** declares a static const variable CHECK_OUT_LENGTH and
** initializes it to 21.
***********************************************************/

#ifndef BOOK_HPP
#define BOOK_HPP
#include "LibraryItem.hpp"
#include <string>

class Book :public LibraryItem {
	//member variable
private:
	std::string author;
public:
	static const int CHECK_OUT_LENGTH = 21;
	//member functions
	Book(std::string, std::string, std::string);
	std::string getAuthor();
	virtual int getCheckOutLength();
};

#endif 

