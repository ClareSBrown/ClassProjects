all: lib

lib: libMain.o  Library.o LibraryItem.o Patron.o Movie.o Album.o Book.o
	g++ -g libMain.o Library.o LibraryItem.o Patron.o Movie.o Album.o Book.o -o lib

libMain.o: libMain.cpp Library.hpp LibraryItem.hpp Patron.hpp Movie.hpp Album.hpp Book.hpp
	g++ -c libMain.cpp

Library.o: Library.cpp Library.hpp LibraryItem.hpp Patron.hpp Movie.hpp Album.hpp Book.hpp
	g++ -c Library.cpp

Patron.o: Patron.cpp Patron.hpp LibraryItem.hpp
	g++ -c Patron.cpp

Movie.o: Movie.cpp Movie.hpp LibraryItem.hpp
	g++ -c Movie.cpp

Album.o: Album.cpp Album.hpp LibraryItem.hpp
	g++ -c Album.cpp

Book.o: Book.cpp Book.hpp LibraryItem.hpp
	g++ -c Book.cpp

LibraryItem.o: LibraryItem.cpp LibraryItem.hpp
	g++ -c LibraryItem.cpp


clean:
	rm -f *.o
