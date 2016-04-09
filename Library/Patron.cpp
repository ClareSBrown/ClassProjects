/**********************************************************
** Author: Clare Brown
** Date: 3/4/2016
** Description: Patron implementation file. Includes definitions
** for a constructor, get functions, addLibraryItem,
** removeLibraryItem, and amendFineAmount.
***********************************************************/
#include "Patron.hpp"
#include <algorithm>


/**********************************************************
** Description: Constructor that takes two string arguments.
** Sets the first string to idNum and the second string to 
** name.
***********************************************************/
Patron::Patron(std::string idn, std::string n) {
	idNum = idn;
	name = n;
	fineAmount = 0.0;

}


//get functions
std::string Patron::getIdNum() {
	return idNum;
}


std::string Patron::getName() {
	return name;
}


std::vector<LibraryItem*> Patron::getCheckedOutItems() {
	return checkedOutItems;
}


double Patron::getFineAmount() {
	return fineAmount;
}


/**********************************************************
** Description: addLibraryItem function. Takes a pointer
** to a LibraryItem as an argument. Adds that LibraryItem
** object to the checkedOutItems vector.
***********************************************************/
void Patron::addLibraryItem(LibraryItem* b) {
	checkedOutItems.push_back(b);
}


/**********************************************************
** Description: removeLibraryItem function. Takes a pointer
** to a LibraryItem object as an argument. Moves through
** checkedOutItems vector until it finds the item and removes
** the item from the vector.
***********************************************************/
void Patron::removeLibraryItem(LibraryItem* b) {
	std::vector<LibraryItem*>::iterator iter = checkedOutItems.begin(); //setting up iterator
	for (int i = 0; i < checkedOutItems.size(); i++) {
		if (checkedOutItems.at(i) == b)//testing if item in vector is the same as the argument
			checkedOutItems.erase(iter);
		else
			iter++; //shifting iterator up 1
	}
}


/**********************************************************
** Description: amendFine function. Takes a double as an 
** argument that represents the amount to add to the fine.
** Adds that number to fineAmount.
***********************************************************/
void Patron::amendFine(double amount) {
	fineAmount += amount;
}