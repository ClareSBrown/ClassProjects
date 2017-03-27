#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]){
	if (argc < 2){
		fprintf(stderr, "Usage: %s keylength", argv[0]);
		exit(1);
	}
	int size = atoi(argv[1]);
	int i, r;
	time_t t;
	srand((unsigned)time(&t));

	const char letters[27] = {' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K','L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
	
	for (i=0; i < size; i++){
		r = rand() % 27;
		fprintf(stdout, "%c", letters[r]);
	}
	fprintf(stdout, "\n");
	return 0;
}
