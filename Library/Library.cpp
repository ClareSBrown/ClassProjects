/**********************************************************
** Author: Clare Brown
** Date: 3/4/2016
** Description: Library class implementation file. Defines
** a default constructor, an addLibraryItem function, an
** addPatron function, a checkOutLibraryItem function, a
** returnLibraryItem function, a requestLibraryItem function,
** a payFine function, an incrementCurrentDate function, 
** a getPatron function, and a getLibraryItem function.
***********************************************************/
#include "Library.hpp"
#include <cmath>


/**********************************************************
** Description: Default constructor. Initializes the currentDate
** to 0.
***********************************************************/
Library::Library() {
	currentDate = 0;
}


/**********************************************************
** Description: addLibraryItem function. Takes a pointer to a 
** LibraryItem object and adds it to the holdings vector.
***********************************************************/
void Library::addLibraryItem(LibraryItem* b) {
	holdings.push_back(b);
}


/**********************************************************
** Description: addPatron function. Takes a pointer to a 
** Patron object and adds it to the members vector.
***********************************************************/
void Library::addPatron(Patron* p) {
	members.push_back(p);
}

/**********************************************************
** Description: checkOutLibraryItem function. Takes two
** strings as parameters, the first referencing a patronID
** and the second referencing an itemID. If specified LibaryItem 
** is not in Library, returns "Item not found". If specified Patron 
** is not in Library, returns "Patron not found". If specified item 
** is already checked out, requests item and returns "Item already
** checked out". If specified libraryItem is on hold, returns 
** "Item on hold by other patron". Else, updates the LibaryItem's 
** checkedoutby, datecheckedout, and location variables with 
** set functions. If library item was on hold for this patron, 
** updates the LibraryItem's requestedBy, updates patron's 
** checkedOutItems and returns "Check out successful".
***********************************************************/
std::string Library::checkOutLibraryItem(std::string patronID, std::string ItemID) {
	//using parameters to get corresponding objects
	LibraryItem *item = getLibraryItem(ItemID);
	Patron *pat = getPatron(patronID);
	//testing if item exists
	if (item == NULL)
		return "Item not found.";
	//testing if patron exists
	else if (pat == NULL)
		return "Patron not found.";
	else {
		Locale location = item->getLocation(); //getting location of item
		if (location == CHECKED_OUT) {//testing if item is checked out
			this->requestLibraryItem(patronID, ItemID); //requesting item if it is already checked out
			return "Item already checked out";
		}
		else if (location == ON_HOLD_SHELF && item->getRequestedBy() != pat)//testing if item is on hold already and if it is on hold by patron
			return "Item on hold by other patron.";
		else {
			//checkout is allowed
			item->setCheckedOutBy(pat);
			item->setDateCheckedOut(currentDate);
			item->setLocation(CHECKED_OUT);//changing location
			//updating requested by if it was requested by patron
			if (item->getRequestedBy() == pat) {
				item->setRequestedBy(NULL);
			}
			pat->addLibraryItem(item); //adding item to patrons vector of items
			return "Check out successful.";
		}
	}
}


/**********************************************************
** Description: returnLibraryItem function. Takes a string
** that indicates the itemId as a parameter. If the specified
** item is not in library, returns "Item not found". If the
** LibraryItem is not checked out, returns "Item already 
** in library". Else, removes item from the patron's checkedOutItems
** vector, updates the LibraryItem's location to either ON_SHELF
** or ON_HOLD_SHELF, and updates the LibraryItem's checkedOutBy.
** Returns "Return successful".
***********************************************************/
std::string Library::returnLibraryItem(std::string ItemID) {
	LibraryItem *item = getLibraryItem(ItemID);
	//checking if Item is part of the library
	if (item == NULL)
		return "Item not found.";
	else { //checking if the library is already in the library
		Locale location = item->getLocation();
		if (location == ON_SHELF || location == ON_HOLD_SHELF)
			return "Item already in library.";
	}
	Patron *pat = item->getCheckedOutBy();
	pat->removeLibraryItem(item); //removing item from patrons list
	item->setCheckedOutBy(NULL); //changing checkedOutBy to NULL
	if (item->getRequestedBy() != NULL) //if requested, put on hold shelf
		item->setLocation(ON_HOLD_SHELF);
	else
		item->setLocation(ON_SHELF);
	return "Return successful.";
}


/**********************************************************
** Description: requestLibraryItem function. Takes two string 
** parameters with the first representing the patronID and the
** second representing the ItemID. If the item is not in the
** library, returns "Item not found." If the patron is not
** in the library, returns "Patron not found." If the item
** is already on hold, returns "Item already on hold." Else
** updates the items requestedBy and it's location to ON_HOLD_SHELF
** unless it is already checked out. Returns request successful.
***********************************************************/
std::string Library::requestLibraryItem(std::string patronID, std::string ItemID) {
	LibraryItem *item = getLibraryItem(ItemID);
	Patron *pat = getPatron(patronID);
	if (item == NULL) //checking if item is in library
		return "Item not found.";
	else if (pat == NULL)//checking if patron is in library
		return "Patron not found";
	else if (item->getRequestedBy() != NULL) //checking if it is already on hold
		return "Item alread on hold.";
	else {
		item->setRequestedBy(pat); //updating requestedBy
		Locale location = item->getLocation();
		if (location != CHECKED_OUT) //if not checked out, set location to ON_HOLD_SHELF
			item->setLocation(ON_HOLD_SHELF);
		return "Request successful.";
	}
}


/**********************************************************
** Description: payFine function. Takes a string and a double as
** arguments. The string represents the patronID and the double
** represents the payment to be made. If patron is not found, 
** returns "Patron not found." Else, uses amendFine to remove 
** the payment from the patrons fineAmount. Returns "Payment
** successful."
***********************************************************/
std::string Library::payFine(std::string patronID, double payment) {
	Patron *pat = getPatron(patronID);
	if (pat == NULL)
		return "Patron not found.";
	else {
		if (payment < 0)
			pat->amendFine(payment); //if payment is negative for some reason
		else
			pat->amendFine(-payment); //using to take away the payment from fine
	}
		
	return "Payment successful.";
}

/**********************************************************
** Description: incrementCurrentDate function. Function with
** no parameters that increments the currentDate each time it
** is run. Moves through all the patrons in the library and
** adds 10 cents to their fine for each item that is overdue.
***********************************************************/
void Library::incrementCurrentDate() {
	currentDate = currentDate + 1; //incrementing date
	int mSize = members.size();
	Patron *pat = NULL;
	double daysOver = 0;
	//Moving through all patrons
	for (int i = 0; i < mSize; i++) {
		pat = members.at(i);
		std::vector <LibraryItem*> itemVec = pat->getCheckedOutItems(); //getting vector with checked out items
		int iSize = itemVec.size();
		//moving through checkedOutItems for each patron
		for (int j = 0; j < iSize; j++) {
			LibraryItem *item = itemVec.at(j); //getting item
			int daysCheckedOut = currentDate - item->getDateCheckedOut(); 
			int checkOutLength = item->getCheckOutLength();
			//testing if item has been checked out longer than its CHECK_OUT_LENGTH
			if (daysCheckedOut > checkOutLength)
				pat->amendFine(.10);
		}
	}
}


/**********************************************************
** Description: getPatron function. Takes a string representing
** the patronID and returns a pointer to the corresponding
** Patron. returns NULL if there is no such Patron in the library.
***********************************************************/
Patron* Library::getPatron(std::string patronID) {
	std::string id;
	Patron* pat = NULL;
	int size = members.size();
	//moving through members vector to find patron
	for (int i = 0; i < size; i++) {
		pat = members.at(i);
		if (pat->getIdNum() == patronID)
			return pat; //returning Patron
		else
			pat = NULL;
	}
	return pat;
}


/**********************************************************
** Description: getLibraryItem function. Takes a string representing
** the ItemID and returns a pointer to the corresponding
** LibraryItem. returns NULL if there is no such LibraryItem 
** in the library.
***********************************************************/
LibraryItem* Library::getLibraryItem(std::string ItemID) {
	std::string id;
	LibraryItem* libItem = NULL;
	int size = holdings.size();
	//moving through vector to find item
	for (int i = 0; i < size; i++) {
		libItem = holdings.at(i);
		if (libItem->getIdCode() == ItemID)
			return libItem; //item is found
		else
			libItem = NULL;
	}
	return libItem;
}
