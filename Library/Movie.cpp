/**********************************************************
** Author: Clare Brown
** Date: 3/4/2016
** Description: Movie class implementatin file.
** Defines a constructor with 3 arguments, a getDirector()
** function and an override of the pure virtual
** getCheckOutLength() function from LibraryItem.
***********************************************************/
#include "Movie.hpp"

/**********************************************************
** Description: Constructor that takes 3 strings as arguments
** and passes the first two to the LibraryItem constructor.
** Sets the third parameter to the director member variable.
***********************************************************/
Movie::Movie(std::string id, std::string t, std::string director):
	LibraryItem::LibraryItem(id, t) {
	this->director = director;
}


//get function: returns the string director
std::string Movie::getDirector() {
	return director;
}


// get function: returns the CHECK_OUT_LENGTH
int Movie::getCheckOutLength() {
	return CHECK_OUT_LENGTH;
}