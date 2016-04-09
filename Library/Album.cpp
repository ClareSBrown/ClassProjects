/**********************************************************
** Author: Clare Brown
** Date: 3/4/2016
** Description: Album class implementatin file. 
** Defines a constructor with 3 arguments, a getArtist()
** function and an override of the pure virtual
** getCheckOutLength() function from LibraryItem. 
***********************************************************/
#include "Album.hpp"

/**********************************************************
** Description: Constructor that takes 3 strings as arguments
** and passes the first two to the LibraryItem constructor.
** Sets the third parameter to the artist member variable. 
***********************************************************/
Album::Album (std::string id, std::string t, std::string artist):
	LibraryItem::LibraryItem(id, t) {
	this->artist = artist;
}


//get function: returns the artist
std::string Album::getArtist() {
	return artist;
}


//get function: returns the CHECK_OUT_LENGTH
int Album::getCheckOutLength() {
	return CHECK_OUT_LENGTH;
}