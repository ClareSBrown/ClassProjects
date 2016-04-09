/**********************************************************
** Author: Clare Brown
** Date: 3/4/2016
** Description: Album class declaration file. Class 
** inherits from the LibraryItem class. Contains a member
** variable that is a string to hold the artist of the Album.
** Declares a constructor with 3 arguments, a getArtist() 
** function and an override of the pure virtual 
** getCheckOutLength() function from LibraryItem. Also
** declares a static const variable CHECK_OUT_LENGTH and 
** initializes it to 14.
***********************************************************/
#ifndef ALBUM_HPP
#define ALBUM_HPP
#include "LibraryItem.hpp"
#include <string>

class Album :public LibraryItem {
	//member variables
private:
	std::string artist;
public:
	static const int CHECK_OUT_LENGTH = 14;
	//member functions
	Album(std::string, std::string, std::string);
	std::string getArtist();
	virtual int getCheckOutLength();

};

#endif 

