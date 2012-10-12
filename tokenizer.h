#ifndef tokenizer_gaurd
#define tokenizer_gaurd

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
//Strictly for determining if a given input is of one of the following
#define NORMAL 0
#define BACKGROUND 1
#define FILE_REDIR 2
#define PIPE 3

char *tokenizer(char*, int); 	//returns an allocated string of the next distinct "word" delimited by mode
int countToNext(char*, int);	//counts the number of characters to the next space/
int contains(char*, char*);		//returns the count of delimiters found in input
int indexOf(char*, char);		//returns the the 1st occurence of a given char in string
int countDistinctStrings(char*, int); //returns the number of distinct (seperated) strings given mode in input
#endif
