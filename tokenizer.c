#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "tokenizer.h"

/**
 * Given input and mode, returns the next "distinct" string delimited by mode
 */
char *tokenizer(char *src, int mode) {
	if (*src == '\0') {
		return NULL ;
	} else {
		int len;
		len = countToNext(src, mode);  //count how many chars to malloc
		if (len == 0)
			return NULL ;
		char *ret = malloc(len + 1);
		int i;
		for (i = 0; i < len; i++) {	  //strcpy len chars
			ret[i] = src[i];
		}
		ret[i] = '\0';
		return ret;
	}
}
/**
 * Counts the number of distinct strings in s given mode
 * E.g: s="ls -l", mode = normal, returns 2.
 * s="ls -l|wc", mode=pipe, returns 2.
 * s="ls -l > ls.txt, mode = redirect, returns 2.
 * s="cat abc.txt def.txt &, mode=background, return 3.
 */
int countDistinctStrings(char*s, int mode) {
	int chars_read, count = 0;
	if (mode == NORMAL)
		while ((chars_read = countToNext(s, mode))) { // there are still "words" in s
			count++;								  // count that word
			s += chars_read;						  // move s up by characters counted
			while (isspace(*s) && *s != '\0')		  // move s until next non space char
				s++;
		}
	else if (mode == PIPE) {
		while ((chars_read = countToNext(s, mode))) {
			count++;
			s += chars_read;
			while ((isspace(*s)) && *s != '|' && *s != '\0') //stop at null or '|'
				s++;
		}
	} else if (mode == FILE_REDIR) {
		while ((chars_read = countToNext(s, mode))) {
			count++;
			s += chars_read;
			while ((isspace(*s)) && (*s != '<' || *s != '>') && *s != '\0') //stop at '<', '>', or null
				s++;
		}
	} else {
		while ((chars_read = countToNext(s, mode))) {
			count++;
			s += chars_read;
			while ((isspace(*s) || *s == '&') && *s != '\0') //skip spaces and & terminator
				s++;
		}
	}
	return count;
}
/**
 Given a string, returns the number of characters to the next delimiter specified by mode
 Pointer movement is managed outside prior to calling this function
 */
int countToNext(char *s, int mode) {
	int len = 0;
	if (mode == NORMAL) {
		while (!isspace(*s) && *s != '\0') { //stop at next space or null
			len++;
			s++;
		}
	} else if (mode == PIPE) {
		while (!isspace(*s) && *s != '|' && *s != '\0') { //stop at next space,| or null
			len++;
			s++;
		}
	} else if (mode == BACKGROUND) {
		while (!isspace(*s) && *s != '&' && *s != '\0') { //stop at next space, & or null
			len++;
			s++;
		}
	} else {
		while (!isspace(*s) && *s != '<' && *s != '>' && *s != '\0') { //stop at next space,<,>,or null
			len++;
			s++;
		}
	}
	return len;
}

/**
 Checks to see if s contains any chars in c. returns its count or 0 if c dne in s.
 */
int contains(char *s, char *c) {
	int ret = 0;
	int i;
	while (*s != '\0') {
		for (i = 0; c[i] != '\0'; i++) { //try to match any of the delimiters in c
			if (*s == c[i])
				ret++;
		}
		s++;
	}
	return ret;
}
/**
 *Returns the 1st index of c in s or -1 if not found.
 */
int indexOf(char *s, char c) {
	int i = -1;
	int j = 1;
	for (; s[i + j] != '\0'; j++) {
		if (s[i + j] == c)
			return i + j;
	}
	return i;
}
