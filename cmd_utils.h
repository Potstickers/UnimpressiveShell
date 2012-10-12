#ifndef cmd_utils_guard
#define cmd_utils guard

#include <unistd.h>
#include <sys/wait.h>
#include "tokenizer.h"
/**
 * Handles tasks related to command structures. (construction, deconstruction, printing)
 */

//Parsed command line object
typedef struct CMD {
	//path if user entered /'s
	char *path;
	//argv array
	char **arg_v;
	//if redirection change to 1s
	int REDIR_STDIN, REDIR_STDOUT;
	//names of files to
	char *FILE_IN, *FILE_OUT;
	//if background, change this to 1
	int isBG;
} Command;

Command **parsePiped(char*, int*); //parses input containing pipes into an array of commands
Command *parseInput(char*); //Parses given input line into Command struct
void freeCMD(Command*);  //frees memory used by the given Command
void freeCMDa(Command**); //frees array of Commands
void printCMD(Command*); //prints all fields in Command
void printCMDa(Command**); //prints array of Commands
char *getBinName(char*); //returns the name of the binary from path
#endif
