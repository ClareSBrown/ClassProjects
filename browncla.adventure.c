/*
* Program 2: Adventure
* Author: Clare Brown
* ONID: browncla
* Date: 10/24/16
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>

#define NUMBER_OF_ROOMS 7
#define MAX_CONNECTIONS 6

#ifndef ROOM_H
#define ROOM_H

/* 
* The Room struct is used when creating the rooms.  All the names, connections, and types
* are stored in a Room struct before being placed into the files representing the rooms
*/
struct Room {
		int id;
		char name[128];
		int connected; //number of connections so far
		int connectionIDs[MAX_CONNECTIONS]; //array to hold which room it is connected to
		char type[128];
};


struct Room* createRoom(int id, const char *name);
void deleteRoom(struct Room* room);
int idConnectionCheck(int check, struct Room* room); 
void printRoom(struct Room *room);
void roomsInit();

#endif

#ifndef GAMEROOM_H
#define GAMEROOM_H

/* 
* The GameRoom struct is used when playing the game. The information from each file is loaded
* into a GameRoom struct which is then referenced while playing the game.  
*/
struct GameRoom {
	int id;
	char name[128];
	int connected;
	char *connectionNames[MAX_CONNECTIONS];
	char type[128];
};

struct GameRoom* createGRoom(int id, const char *name);
void deleteGRoom(struct GameRoom* gRoom);
int getid(char **names , char * temp);
int nameConnectionCheck(char * temp, struct GameRoom *tempGRoom);
void printGRoom(struct GameRoom *gRoom);
void loadGame(struct GameRoom **gRoomArr, int * current_id, int * end_id, char ** roomNames);

#endif

/****************************Functions involving Room structs****************************/

/*
* A function to create a Room struct. Allocates memory for the struct. Sets id and name. 
* Sets all the values in the connections arrays to -1, and sets the room type to MID_ROOM. 
* Parameters: 
* 	-int representing id of the room
* 	-pointer to string representing name of room
* Returns: 
* 	-pointer to a Room struct
*/
struct Room* createRoom(int id, const char* name){
	int i;
	struct Room *newRoom = malloc(sizeof(struct Room));
	newRoom->id = id;
	strcpy(newRoom->name, name);
	for (i = 0; i < MAX_CONNECTIONS; i++){
		newRoom->connectionIDs[i] = -1;
	}
	newRoom->connected = 0;
	strcpy(newRoom->type, "MID_ROOM"); //assigning all rooms to mid rooms 
	return newRoom;
}


/*
* A function to delete a Room struct. Frees the memory allocated for the struct.
* Parameters: 
* 	-Pointer to a Room struct
* Returns:
* 	-void
*/
void deleteRoom(struct Room* room){
	free(room);
}


/*
* A function to check if the given ID already has a connection to the given Room. 
* Parameters: 
* 	-int representing id to be checked
* 	-pointer to a Room Struct to be checked
* Returns: 
* 	-0 if connection does not exist, 1 if connection exists
*/
int idConnectionCheck(int check, struct Room* room){
	int i;
	for (i =0; i < room->connected; i++){
		if (room->connectionIDs[i] == check)
			return 1;
	}
	return 0;
}


/*
* A function to print all of the information stored in a Room struct for 
* debugging purposes.
* Parameters: 
* 	-pointer to a Room struct
* Returns:
* 	-void
*/
void printRoom(struct Room *tempRoom){
	int i;
	printf("ID: %d\n", tempRoom->id);
	printf("Name: %s\n", tempRoom->name);
	for (i = 0; i < tempRoom->connected; i++){
		printf("Connection %d: %s\n", i, tempRoom->connectionIDs[i]);
	}
	printf("Type: %s\n", tempRoom->type);
}


/*
* A function that sets up the directory and room files for later use in the adventure
* game. Creates a directory and uses and array of pointers to Room structs to gather 
* information before putting them in the files. The id of each Room struct corresponds 
* to the index in the array of pointers to Room structs
* Parameters:
*  	-none
* Returns: 
* 	-void
*/
void roomsInit(){
	//Variable declarations
	int i, j, k, r, num;
	FILE *f1;
	struct Room *tempRoom;
	char filepath[256], directoryName[256];
	char tempString[50]; //string for various uses
	struct Room *usedRooms[NUMBER_OF_ROOMS]; //array to hold pointers to structs

	//Array of possible names
	const char *roomNames[10]={"Armory", "GreatHall", "Barracks", "MessHall", "ThroneRoom", "Ramparts", "Drawbridge", "Kitchen", "Stables", "Dungeon"};	

	//Creating directory
	int pid = getpid();
	char *startDir = "browncla.rooms.";
	snprintf(directoryName, 256, "%s%d", startDir, pid);
	struct stat st = {0};
	if (stat(directoryName, &st) == -1){
		if (mkdir(directoryName, 0755) == -1)
			perror("mkdir: ");
	}

	//Creating room files
	srand(time(NULL)); //seeding the random number generator based on time
	for (i = 0; i < NUMBER_OF_ROOMS; i++){
		sprintf(filepath, "%s/room%d.txt", directoryName, i);
		f1= fopen(filepath, "w");
		if (f1 == NULL)
			perror("Error opening file");
		else{
			r = rand()%10; //getting random number between 0 and 9 to pick name

			//Checking if room is NULL (already taken). If it is, get another random number.
			while (roomNames[r] == NULL){
				r=rand()%10;		
			}
			tempRoom = createRoom(i, roomNames[r]); //creating Room struct with id and roomName
			usedRooms[i]=tempRoom; //storing struct in array of structs
			roomNames[r] = NULL; //setting spot in array of names to NULL so that it cannot be used again
		}
		fclose(f1);
	}
	
	//Making connections. i is the room that is making the connections, k is the room that
	//is being connected to. 
	for (i=0; i < NUMBER_OF_ROOMS; i++){
		num = usedRooms[i]->connected; //getting number of connections already there for the Room

		//only add connections if less than 3 currently in room (Helps make the spread of connections more varied)
		if (num < 3){
			r = (rand()%4)+3; //getting the number of connections for the room (between 3 and 6)

			//Adding connections until have r connections in the room
			for (j=num; j < r; j++){

				k = rand()%NUMBER_OF_ROOMS; //getting random number for id of room to connect to

				//Making sure current room isn't connecting to itself or previous connection
				while (k == i || idConnectionCheck(k, usedRooms[i])){
					k=rand()%NUMBER_OF_ROOMS;
				}

				//Storing id in arrays of current room and room to be connected room
				usedRooms[i]->connectionIDs[j]=usedRooms[k]->id;
				usedRooms[k]->connectionIDs[usedRooms[k]->connected]=usedRooms[i]->id;

				//Incrementing the number of connected rooms for each of the rooms
				usedRooms[k]->connected = usedRooms[k]->connected + 1;
				usedRooms[i]->connected = usedRooms[i]->connected + 1;
			}
		}

	}

	//Setting start and end room by getting two random rooms
	k = rand()%NUMBER_OF_ROOMS;
	r = rand()%NUMBER_OF_ROOMS;
	while (k == r){
		r = rand()%NUMBER_OF_ROOMS;
	}
	strcpy(usedRooms[k]->type, "START_ROOM");
	strcpy(usedRooms[r]->type,  "END_ROOM");
	
	//Putting the information in the files
	for (i = 0; i < NUMBER_OF_ROOMS; i++){	
		sprintf(filepath, "%s/room%d.txt", directoryName, i); //getting beginning of path
		f1 = fopen(filepath, "a+"); //opening file
		if (f1 == NULL)
			perror("Error opening file");
		else{
			//Creating first line and adding it to the file
			sprintf(tempString, "ROOM NAME: %s\n", usedRooms[i]->name);
			fputs(tempString, f1);

			//Creating next 3-6 lines (connections) and appending to the file
			for (j = 0; j < usedRooms[i]->connected; j++){
				sprintf(tempString, "CONNECTION %d: %s\n",j+1,usedRooms[usedRooms[i]->connectionIDs[j]]->name);
				fputs(tempString, f1);
			}

			//Creating last line and appending it to the file
			sprintf(tempString, "ROOM TYPE: %s\n", usedRooms[i]->type);
			fputs(tempString,f1);
		}
		fclose(f1);
	}


	//Clearing up memory after information stored in files
	for (i = 0; i < NUMBER_OF_ROOMS; i++){
		deleteRoom(usedRooms[i]);
	}
}




/****************************Functions involving GameRoom structs****************************/

/*
* A function to create a GameRoom struct. Allocates memory for the struct. Sets id and name. 
* Allocates memory for all the strings in connectionNames. 
* Parameters: 
* 	-int representing id of the room
* 	-pointer to string representing name of room
* Returns: 
* 	-pointer to a GameRoom struct
*/
struct GameRoom* createGRoom(int id, const char* name){
	int i;
	struct GameRoom *newGRoom = malloc(sizeof(struct GameRoom));
	newGRoom->id = id;     

	char * connectedRooms[MAX_CONNECTIONS];
	for (i = 0; i < MAX_CONNECTIONS; i++){
		connectedRooms[i] = malloc(128 * sizeof (char));
		newGRoom->connectionNames[i] = connectedRooms[i];
	}
	strcpy(newGRoom->name, name); 
	
	return newGRoom;
}


/*
* A function to delete a GameRoom struct. Frees the memory allocated for the array of 
* C style strings and the memory allocated for the struct.
* Parameters: 
* 	-Pointer to a GameRoom struct
* Returns:
* 	-void
*/
void deleteGRoom(struct GameRoom* gRoom){
	int i;
	for (i = 0; i < MAX_CONNECTIONS; i++){
		free(gRoom->connectionNames[i]);
	}
	free(gRoom);
}


/*
* A function to get the id of the GameRoom with the given name.
* Parameters:
* 	-Pointer to array of string with all the names of the GameRooms
* 	-Pointer to string representing the name to be found
* Returns:
* 	-int representing the id of the GameRoom
* 	-returns -1 if not found
*/
//Function to return ID of room
int getid(char **names , char * temp){
	int i;
	for (i = 0; i < NUMBER_OF_ROOMS; i++){
		if (strcmp(names[i], temp)==0)
			return i;
	}
	return -1;
}


/*
* A function to check if the given name is a connection to the given GameRoom. 
* Parameters: 
* 	-pointer to string representing name to be checked
* 	-pointer to a GameRoom Struct to be checked
* Returns: 
* 	-0 if connection does not exist, 1 if connection exists
*/
int nameConnectionCheck(char * temp, struct GameRoom *tempGRoom){
	int i, j;
	j = tempGRoom->connected;
	for (i = 0; i < j; i++){
		if (strcmp(temp, tempGRoom->connectionNames[i])==0){
			return 1;
		}
	}
	return 0;
}


/*
* A function to print all of the information stored in a GameRoom struct for 
* debugging purposes.
* Parameters: 
* 	-pointer to a GameRoom struct
* Returns:
* 	-void
*/
void printGRoom(struct GameRoom *tempGRoom){
	int i;
	printf("ID: %d\n", tempGRoom->id);
	printf("Name: %s\n", tempGRoom->name);
	for (i = 0; i < tempGRoom->connected; i++){
		printf("Connection %d: %s\n", i, tempGRoom->connectionNames[i]);
	}
	printf("Type: %s\n", tempGRoom->type);
}


/*
* A function that loads the contents of each of the room files stored in the directory 
* browncal.rooms.<pid>. The information is stored in the array of pointers to GameRoom
* structs that is passed as a parameter. The names of each of the rooms is stored in the
* array of C style strings passed as a parameter with each index representing the id of the
* GameRoom with that name. The id of the GameRoom that has type START_ROOM is stored in the location
* pointed at by the current_id parameter and the id of the GameRoom that has type END_ROOM
* is stored in the location pointed at by the end_id parameter.
* sets up the directory and room files for later use in the adventure
* Parameters:
*  	-Array of pointers to GameRoom structs to store newly created structs
* 	-address of int to store the start room's id
* 	-address of int to store the end room's id
* 	-Array of C style strings to store the names of the rooms in corresponding indices
* Returns: 
* 	-void
*/
void loadGame(struct GameRoom **gRoomArr, int * current_id, int * end_id, char ** roomNames){
	//Variable declarations
	int i, j, k, connections, stepCount;
	char c;
	FILE *f1, *tempFile;
	struct GameRoom *tempGRoom; //temp GameRoom to use to put rooms in array
	char filepath[256], directoryName[256];
	char tempString[50];
	char *temp;

	//Getting path for files
	int pid = getpid();
	char *startDir = "browncla.rooms.";
	snprintf(directoryName, 256, "%s%d", startDir, pid);

	//Opening each file and reading information into struct
	for (i = 0; i < NUMBER_OF_ROOMS; i++){
		sprintf(filepath, "%s/room%d.txt", directoryName, i);
		f1= fopen(filepath, "r+");
		if (f1 == NULL) 
			perror("Error opening file");
		else {

			//Reading first line. Moving 11 characters in to account for first part of line (ROOM NAME: )
			for (j = 0; j < 11; j++){
				c=fgetc(f1);
			}
			//Getting remainder of line and using it to create GameRoom struct.
			if (fgets(tempString, 128, f1) != NULL){
				temp = strtok(tempString, "\n");
				tempGRoom = createGRoom(i, tempString);
				gRoomArr[i] = tempGRoom;
				roomNames[i]=malloc(128 * sizeof (char));
				strcpy(roomNames[i], tempString); //storing names roomNames array in index of ID for later use
			}

			//Getting connections, looping through lines until first character of next line is R. 
			//If next line starts with R, have reached the line ROOM TYPE and all connections have been entered
			k=0; 			//counter for number of connections
			c = fgetc(f1); //getting first character of next line
			
			while (c == 67){
				//Moving 13 characters in to account for first part of line after the first character (ONNECTION 1: )
				for (j = 0; j < 13; j++){
					c = fgetc(f1);
				}	

				//Getting remainder of line and storing it connectionNames array
				if (fgets(tempString, 128, f1) !=NULL){
					temp = strtok(tempString, "\n");
					strcpy(tempGRoom->connectionNames[k], tempString);
					k++;
				}
				tempGRoom->connected = k; //storing the number of connections in the GameRoom struct
				c = fgetc(f1);	
			}

			//Getting Room Type. Moving 10 charcters in to account for first part of line minus the first character (OOM TYPE: )
			for (j = 0; j < 10; j++){
				c=fgetc(f1);
			}

			//Getting remainder of th e line and storing it in type variable of GameRoom struct
			if (fgets(tempString, 128, f1) != NULL){
				temp = strtok(tempString, "\n");
				strcpy(tempGRoom->type, tempString);

				//storing current_id and end_id for use in game
				if (strcmp(tempString, "START_ROOM") == 0){
					*current_id = i;
				}
				else if (strcmp(tempString, "END_ROOM") ==0){
					*end_id = i;
				}
				
			}
		}
		fclose(f1); //closing file
	}
}



int main(){
	
	//Variable declarations
	int i, current_id, end_id, connections, pid, stepCount;
	FILE *tempFile;
	struct GameRoom *tempGRoom;
	char directoryName[256];
	char tempString[128], tempFileName[128];
	char *temp;
	char *roomNames[NUMBER_OF_ROOMS]; //array to hold names of rooms for easier access
	struct GameRoom *gRoomArr[NUMBER_OF_ROOMS]; //array to hold pointers to structs

	//Setting up directory and rooms
	roomsInit();

	//Loading everything into structs to play game
	loadGame(gRoomArr, &current_id, &end_id, roomNames);
	
	//Setting up count to count number of steps
	stepCount = 0;

	//Setting up temp file to store path
	pid = getpid();
	char *startDir = "browncla.rooms.";
	snprintf(directoryName, 256, "%s%d", startDir, pid);
	sprintf(tempFileName, "%s/temp.txt", directoryName);

	//Starting the Game
	printf("Welcome to the Maze...\n");

	//Game runs while the id of the room the user is at does not equal the id of the end room.
	while (current_id != end_id){

		//printing current location
		printf("\nCURRENT LOCATION: %s\n", roomNames[current_id]);

		//Print connections
		printf("POSSIBLE CONNECTIONS: ");

		tempGRoom = gRoomArr[current_id]; //getting pointer to current GameRoom
		connections = tempGRoom->connected; //getting number of connections in that Game Room
		
		//Iterating through connectionsNames array of curren GameRoom to print connections
		for (i = 0; i < connections - 1; i++){
			printf("%s, ", tempGRoom->connectionNames[i]);
		}

		printf("%s.\n", tempGRoom->connectionNames[connections-1]);

		//Prompting the user to input name for next room
		printf("WHERE TO? > ");
		scanf("%s", tempString); //getting the entered string
		while(getchar() != '\n'); //clearing buffer

		//If string is valid connection, move to that room
		if (nameConnectionCheck(tempString, tempGRoom) == 1){

			//Get id of next room
			current_id = getid(roomNames, tempString);

			//Store name of next room in temp file for printing path at end of game
			tempFile = fopen(tempFileName, "a");
			if (tempFile == NULL)
				perror("Error opening file");
			else{
				fprintf(tempFile, tempString);
				fprintf(tempFile, "\n");
				fclose(tempFile);

				stepCount++; //incrementing count for number of steps taken for end of game
			}
		}

		//Else print error statement if not a valid room
		else{
				printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
			}
	}

	//End room has been found!
	printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");

	//Printing number of steps
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", stepCount);

	//Opening file that contains the names of the rooms visited to print them
	tempFile = fopen(tempFileName, "r");
	if (tempFile == NULL)
			perror("Error opening file");
	else{
		//Printing names listed in file
		while (fgets(tempString, sizeof(tempString), tempFile)){
			printf("%s", tempString);
		}
	}
	fclose(tempFile);

	//deleting temporary file
	remove(tempFileName);

	//clearing up allocated memory
	for (i = 0; i < NUMBER_OF_ROOMS; i++){
		deleteGRoom(gRoomArr[i]); 
		free(roomNames[i]);
	}
	return 0;
}
