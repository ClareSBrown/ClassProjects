/**********************************************************
** Author: Clare Brown
** Date: 3/4/2016
** Description: Movie class declaration file. Class
** inherits from the LibraryItem class. Contains a member
** variable that is a string to hold the director of the Movie.
** Declares a constructor with 3 arguments, a getDirector()
** function and an override of the pure virtual
** getCheckOutLength() function from LibraryItem. Also
** declares a static const variable CHECK_OUT_LENGTH and
** initializes it to 7.
***********************************************************/

#ifndef MOVIE_HPP
#define MOVIE_HPP
#include "LibraryItem.hpp"
#include <string>

class Movie :public LibraryItem {
	//member variable
private:
	std::string director;
public:
	static const int CHECK_OUT_LENGTH = 7;
	//member functions
	Movie(std::string, std::string, std::string);
	std::string getDirector();
	virtual int getCheckOutLength();

};

#endif 
