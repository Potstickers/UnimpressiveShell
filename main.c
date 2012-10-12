#include "cmd_utils.h"

void cleanInput(char*);

int main() {
	char cwd[100]; //current working directory
	char *input = NULL; //where user input will be stored
	int status, changed_dir = 1; //status of a waited-on process, flag indicating user changed directory
	int n_cmds; //the number of commands if piped
	size_t input_len; //for getline num bytes read
	pid_t pid; //single pid
	Command **arr_cmd = NULL; //array of commands for piped commands
	Command *cmd = NULL; //single command for representing non piped cmd

	while (1) {
		if (changed_dir) { //refresh cwd if cd was entered
			getcwd(cwd, 99);
			changed_dir = 0;
		}
		printf("[WorkingName]@%s>", cwd); //prompt
		if (getline(&input, &input_len, stdin) == -1) //get input
			return 0;
		cleanInput(input); //trim leading&trailing whitespace
		if (strncasecmp(input, "exit", 4) == 0) {
			free(input);
			return 0;
		}
		if (strlen(input) == 0) { //empty input
			printf("SYS:\"Stop wasting cycles...\"\n");
			free(input);
			input = NULL;
			continue;
		}
		printf("You entered: \"%s\"\n", input);
		//Determine if piped. if not, only use *cmd, else **cmd
		if (indexOf(input, '|') != -1) {
			arr_cmd = parsePiped(input, &n_cmds);
		} else {
			cmd = parseInput(input);
		}
		free(input); //no need for this beyond this point
		input = NULL;
		//unpiped command parsed
		if (cmd != NULL ) {
			if (strncmp(cmd->arg_v[0], "cd", 2) == 0) { //the only builtin cmd
				chdir(cmd->arg_v[1]);
				changed_dir = 1;
				freeCMD(cmd);
				cmd = NULL;
				continue;
			}
			printCMD(cmd);
			pid = fork();
			if (pid < 0)
				printf("Error creating new process\n.");
			if (pid == 0) {
				//handle file redirection here
				if (cmd->REDIR_STDIN == 1) {
					int in_fd = fileno(fopen(cmd->FILE_IN, "r"));
					if (in_fd < 0) {
						printf("Error: redirecting stdin failed\n");
					} else {
						dup2(in_fd, 0);
					}
				}
				if (cmd->REDIR_STDOUT == 1) {
					int out_fd = fileno(fopen(cmd->FILE_OUT, "w"));
					if (out_fd < 0) {
						printf("Error: redirecting stdout failed\n");
					} else {
						dup2(out_fd, 1);
					}
				}
				execvp(cmd->path, cmd->arg_v);
				printf("%s: Command failed to execute (or not found)\n",
						cmd->arg_v[0]);
				exit(0); //failed execs must exit their own forks, or risk running out of processes
			}
			if (!cmd->isBG)
				wait(&status);
			else
				signal(SIGCHLD, SIG_IGN ); //zombie prevention
			freeCMD(cmd);
			cmd = NULL;
		}
		//Piped commands parsed
		if (arr_cmd != NULL ) {
			printCMDa(arr_cmd);
			int i, n_pipes = n_cmds - 1;
			//==========Create Pipes====================
			int fds[2 * n_pipes]; 		//2 fds per pipe
			for (i = 0; i < n_pipes; i++) {
				if (pipe(fds + (i * 2)) < 0) {
					perror("Pipe");
					break;
				}
			}
			//==========Execute Commands (See 'Pipes.jpg')================
			int index = 0; //index of in-ends = index*2, index of out-ends = index*2+1
			for (i = 0; i < n_cmds; i++) {
				pid = fork();
				if (pid < 0) {
					perror("Fork");
					break;
				}
				if (pid == 0) {
					if (i > 0) { //current command is not the first command,dup stdin
						if (dup2(fds[index - 2], 0) < 0) {
							perror("dup stdin");
							break;
						}
					}
					if (i < n_pipes) { //current command is not the last command, dup stdout
						if (dup2(fds[index + 1], 1) < 0) {
							perror("Dup stdout");
							break;
						}
					}

					int j;
					for (j = 0; j < 2 * n_pipes; j++) //close all ends
						close(fds[j]);
					execvp(arr_cmd[i]->path, arr_cmd[i]->arg_v);
					printf("Execvp failed\n");
					exit(0);
				}
				index += 2;
			}
			//============ Parent closes fds ===========
			for (i = 0; i < n_pipes * 2; i++)
				close(fds[i]);
			//============ Wait :) =====================
			for (i = 0; i < n_cmds; i++) 
				wait(&status);
			
			freeCMDa(arr_cmd);
			arr_cmd = NULL;
		}
	}
	free(input);
	return 0;
}
/**
 *  trims leading and trailing whitespace given input
 */
void cleanInput(char *input) {
	char *p = input; //traverse ahead pointer
	while (isspace((int)*p)) //leading whitespace
		p++;
	if (*p == '\0') {
		*input = *p;
		return;
	}
	while (*p != '\n') {
		*input++ = *p++;
	}
	while (isspace((int)*(input-1)))
		input--; //trailing whitespace
	*input = '\0';
}