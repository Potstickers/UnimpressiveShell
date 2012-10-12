#include "cmd_utils.h"
/**
* The main parser. Determines what input contains (redirection,background, or pipes)
* and returns a Command * with all fields set.
*/
Command *parseInput(char *input) {
	Command *ret = NULL;
	int num_distinct, i = 0;
	if (input == NULL )
		return NULL ;
	ret = malloc(sizeof(Command));
	ret->path = NULL;
	//========================Background case======================
	if (indexOf(input, '&') == (strlen(input) - 1)) {
		if ((num_distinct = countDistinctStrings(input, BACKGROUND)) == 0) {
			free(ret);
			printf("No command specified\n");
			return NULL ;
		}
		ret->arg_v = malloc((num_distinct + 1) * sizeof(char*));
		while (*input != '\0' && *input != '&') {					//stop at next null or &
			if (ret->path == NULL ) {								//first token reads in path
				ret->path = tokenizer(input, BACKGROUND);
				ret->arg_v[i++] = getBinName(ret->path);			//argv[0] is the binary name after last '/' in path
			} else {
				ret->arg_v[i++] = tokenizer(input, BACKGROUND);		//parse rest of argv
			}
			input += countToNext(input, BACKGROUND);				//move input up 
			while (isspace(*input))
				input++;
		}
		ret->arg_v[i] = NULL;
		ret->REDIR_STDIN = ret->REDIR_STDOUT = 0;
		ret->isBG = 1;
		return ret;
		//=======================Redirection case======================
	} else if (indexOf(input, '>') > 0 || indexOf(input, '<') > 0) {
		if ((num_distinct = countDistinctStrings(input, FILE_REDIR)) == 0) {
			printf("No command specified.\n");
			free(ret);
			return NULL ;
		}
		ret->arg_v = malloc((num_distinct + 1) * sizeof(char*));
		while (*input != '\0' && *input != '<' && *input != '>') {//stop at next '>' or '<'
			if (ret->path == NULL ) {
				ret->path = tokenizer(input, FILE_REDIR);
				ret->arg_v[i++] = getBinName(ret->path);
			} else {
				ret->arg_v[i++] = tokenizer(input, FILE_REDIR);
			}
			input += countToNext(input, FILE_REDIR);
			while (isspace(*input))
				input++;
		}
		ret->arg_v[i] = NULL;
		ret->isBG = 0;
		//set files
		while (*input != '\0') {
			if (*input == '>') {								//found output redir -----
				while (isspace(*(++input)));					//move input to next string (file name)
				ret->FILE_OUT = tokenizer(input, FILE_REDIR);	//set the name to output to
				if(ret->FILE_OUT == NULL) {
					printf("No output file specified.\n");
					ret->REDIR_STDOUT = 0;
				}else{
					ret->REDIR_STDOUT = 1;						//set the flag
				}
				input += countToNext(input, FILE_REDIR);		//move input up
			} else if (*input == '<') {							//found input redir -----						
				while (isspace(*(++input)));					//move input to next string
				ret->FILE_IN = tokenizer(input, FILE_REDIR);	//set the name to input from
				if(ret->FILE_IN == NULL) {
					printf("No input file specified.\n");
					ret->REDIR_STDIN = 0;
				}else{
					ret->REDIR_STDIN = 1;						//set the flag
				}
				input += countToNext(input, FILE_REDIR);		//move input up
			}
			while (isspace(*input))								//skip whitespaces
				input++;
		}
		return ret;
		//======================pipe case=============================
	} else if (indexOf(input, '|') > -1) {
		if ((num_distinct = countDistinctStrings(input, PIPE)) == 0) {  //garbage input
			free(ret);
			return NULL ;
		}
		ret->arg_v = malloc((num_distinct + 1) * sizeof(char*));
		while (*input != '\0' && *input != '|') {						//same procedure as previous cases
			if (ret->path == NULL ) {									//except stops at next '|'
				ret->path = tokenizer(input, PIPE);
				ret->arg_v[i++] = getBinName(ret->path);
			} else {
				ret->arg_v[i++] = tokenizer(input, PIPE);
			}
			input += countToNext(input, PIPE);
			while (isspace(*input))
				input++;
		}
		ret->arg_v[i] = NULL;
		ret->isBG = ret->REDIR_STDIN = ret->REDIR_STDOUT = 0;
		ret->FILE_IN = ret->FILE_OUT = NULL;
		return ret;
	}else{
		//========================Normal foreground command=============
		if ((num_distinct = countDistinctStrings(input, NORMAL)) == 0) {
			free(ret);
			return NULL ;
		}
		ret->arg_v = malloc((num_distinct + 1) * sizeof(char*));
		while (*input != '\0') {
			if (ret->path == NULL ) {
				ret->path = tokenizer(input, NORMAL);
				ret->arg_v[i++] = getBinName(ret->path);
			} else {
				ret->arg_v[i++] = tokenizer(input, NORMAL);
			}
			input += countToNext(input, NORMAL);
			while (isspace(*input))
				input++;
		}
		ret->arg_v[i] = NULL;
		ret->REDIR_STDIN = ret->REDIR_STDOUT = ret->isBG = 0;
		return ret;
	}
	free(ret);
	printf("No command specified.\n");
	return NULL ;
}
/**
* Parses input containing pipes and returns an array of Commands
* and stores command count in cmd_count
*/
Command **parsePiped(char *input, int *cmd_count) {
	int i, num_pipes = contains(input, "|");
	int num_commands = *cmd_count = num_pipes + 1;
	Command **ret = malloc((num_commands + 1) * sizeof(Command*));
	for (i = 0; i < num_commands; i++) {
		ret[i] = parseInput(input);
		if(ret[i]==NULL){							//premature null commands garbage input like: "|||||"
			printf("Null command(s) found. \n");
			*cmd_count = 0;
			freeCMDa(ret);
			return NULL;
		}
		input += indexOf(input, '|');
		while (isspace(*input) || *input == '|')
			input++;
	}
	ret[i] = NULL;
	return ret;
}
/**
* Frees all alloc'ed memory given Command array
*/
void freeCMDa(Command **cmd_arr) {
	int i;
	if(cmd_arr!=NULL){
		for (i = 0; cmd_arr[i] != NULL ; i++)
			freeCMD(cmd_arr[i]);
		free(cmd_arr);
	}
}
/**
* Frees all alloc'ed memory given a Command*
*/
void freeCMD(Command *cmd) {
	int i;
	if(cmd!=NULL){
		free(cmd->path);
		for (i = 1; cmd->arg_v[i] != NULL ; i++) { //start at arg_v[1], arg_v[0] is not malloced
			free(cmd->arg_v[i]);
		}
		if (cmd->REDIR_STDIN == 1)
			free(cmd->FILE_IN);
		if (cmd->REDIR_STDOUT == 1)
			free(cmd->FILE_OUT);
		free(cmd->arg_v);
	}
}
/**
* Prints information on a given Command
*/
void printCMD(Command *cmd) {
	printf("Path: %s Arg_v:", cmd->path);
	int i;
	for (i = 0; cmd->arg_v[i] != NULL ; i++)
		printf(" %s", cmd->arg_v[i]);
	printf("\n");
	if (cmd->REDIR_STDIN)
		printf("Redirect input from: %s\n", cmd->FILE_IN);
	if (cmd->REDIR_STDOUT)
		printf("REdirect output to: %s\n", cmd->FILE_OUT);
	if (cmd->isBG)
		printf("Is a background task.\n");
}
/**
* Prints information of each Command given an array of Commands
*/
void printCMDa(Command **cmd_arr) {
	while (*cmd_arr != NULL ) {
		printCMD(*cmd_arr++);
	}
}
/**
* Returns the name of the binary located in path.
* E.g. if path ="/dir/dir/bin" or "dir/bin", returns "bin"
*/
char *getBinName(char *path) {
	int n;
	while (*path != '\0') {
		n = indexOf(path, '/');
		if (n == 0)
			path++;  	// '/' is right in front
		else if (n < 0)
			break;   	// no more '/'s, name of binary is where it should be
		else if (n > 0)
			path += n; // '/' is n chars away
	}
	return path;
}
