// This program is for reading from and writing into a file by giving command line arguments. 
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <stdlib.h>
using namespace std;

int main (int argc, char *argv[]) {

	int read = 0;  		//To enter in a read loop
	int write = 0; 		//To enter in a write loop
	char *fname;   		//filename
	char *sentence;		//entered string will be stored here
	int repeat;    		//count
	printf("No. of argc = %d\n", argc);
	
	for (int i = 1; i < argc; ++i) {
		printf("\n****\n");
		printf("ARGC = %d\tARGV = %s\n",i,argv[i]);

		if (strcmp(argv[i],"-f") == 0) {
			fname = argv[i+1];
			printf("Save file\nFilename = %s\n",fname);
		} else if (strcmp(argv[i],"-r") == 0) {
			read = 1;
			printf("READ called\n");
		} else if (strcmp(argv[i],"-w") == 0) {
			write = 1;
			printf("WRITE called\n");
		} else if (strcmp(argv[i], "-p") == 0) {
			sentence = argv[i+1];	
			printf("STRING: %s",sentence);
		} else if (strcmp(argv[i],"-n") == 0) {
			repeat = atoi(argv[i+1]);
			printf("COUNT = %d\n",repeat);
		}
	}


   // READ 
	if (read == 1) {
		string line;
		ifstream myfile (fname);
		if (myfile.is_open()) {
			printf("Attempting to read file contents...\n");
			while ( getline (myfile,line) ) {
				cout << line << '\n';
			}
			myfile.close();
		} else cout << "Unable to open file";
	}


	// WRITE
	if (write == 1) {
		printf("Write to file!\n");
		printf("COUNT = %d\n",repeat);
		printf("STRING: %s\n",sentence);
		
		ofstream myfile(fname); 
		for(int i = 0; i < repeat; i++) {
			if(myfile.is_open()){
				  
				myfile << sentence << endl ; 
				cout << sentence << endl; 
				}
			
				else {
					cout << "Unable to write into file" << endl ; 	
				     }
			printf("Repeat count %d\n",i);

                     } 
	}
	return 0;
}
