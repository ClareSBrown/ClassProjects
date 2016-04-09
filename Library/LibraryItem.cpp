/**********************************************************
** Author: Clare Bronw
** Date: 3/4/2016
** Description: LibraryItem implementation file. Contains
** definitions for a constructor and get and set functions 
** for the member variables. There are no set functions
** for idCode and title.
***********************************************************/
#include "LibraryItem.hpp"


/**********************************************************
** Description: Constructor for LibraryItem. Takes two 
** strings as arguments Sets the first argument to the
** idCode and the second argument to the title. Sets 
** dateCheckedOut to 0, the location to ON_SHELF, and both
** requestedBy and checkedOutBy to NULL.
***********************************************************/
LibraryItem::LibraryItem(std::string idc, std::string t) {
	idCode = idc;
	title = t;
	dateCheckedOut = 0;
	checkedOutBy = NULL;
	requestedBy = NULL;
	location = ON_SHELF;
}


//get functions
std::string LibraryItem::getIdCode() {
	return idCode;
}


std::string LibraryItem::getTitle() {
	return title;
}


Locale LibraryItem::getLocation() {
	return location;
}


Patron* LibraryItem::getCheckedOutBy() {
	return checkedOutBy;
}


Patron* LibraryItem::getRequestedBy() {
	return requestedBy;
}


int LibraryItem::getDateCheckedOut() {
	return dateCheckedOut;
}


//set functions
void LibraryItem::setLocation(Locale location) {
	this->location = location;
}


void LibraryItem::setCheckedOutBy(Patron* checkedOutBy) {
	this->checkedOutBy = checkedOutBy;
}


void LibraryItem::setRequestedBy(Patron *requestedBy) {
	this->requestedBy = requestedBy;
}


void LibraryItem::setDateCheckedOut(int dateCheckedOut) {
	this->dateCheckedOut = dateCheckedOut;
}