/**********************************************************
** Author: Clare Brown
** Date: 3/4/2016
** Description: Book class implementatin file.
** Defines a constructor with 3 arguments, a getAuthor()
** function and an override of the pure virtual
** getCheckOutLength() function from LibraryItem.
***********************************************************/
#include "Book.hpp"


/**********************************************************
** Description: Constructor that takes 3 strings as arguments
** and passes the first two to the LibraryItem constructor.
** Sets the third parameter to the author member variable.
***********************************************************/
Book::Book(std::string id, std::string t, std::string author):
	LibraryItem::LibraryItem(id, t){
	this->author = author;
}


//get function: returns the author
std::string Book::getAuthor() {
	return author;
}


//get function: returns CHECK_OUT_LENGTH
int Book::getCheckOutLength() {
	return CHECK_OUT_LENGTH;
}