Compiled with "gcc -o shell *.c *.h"
To run: "shell" or "./shell" depending on terminal
You can redirect input and/or output to files like "shell > output < input file"
provided that any lines read in as a command can be printed out. eg: vim, gedit, or any text editors opened under shell will not work.
The shell can process commands in foreground, background, foreground file redirection, or foreground piping.
Never both at same time.

Next push: 
- clean up memory leaks.
- support &<>| on same line.
- refactor: parses runs over the same areas in input several times [tokenizer: count to next(1); strcpy(2); move input ptr: input+=count to next(3)].
Maybe do a return int, take &ptr as an extra argument in tokenizer.