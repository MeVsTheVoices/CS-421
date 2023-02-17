#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */

 
/**
 * setup() reads in the next command line, separating it into distinct tokens
 * using whitespace as delimiters. setup() sets the args parameter as a
 * null-terminated string.
 */

void setup(char inputBuffer[], char *args[],int *background)
{
    int length, /* # of characters in the command line */
        i,      /* loop index for accessing inputBuffer array */
        start,  /* index where beginning of next command parameter is */
        ct;     /* index of where to place the next parameter into args[] */
   
    ct = 0;

    /* read what the user enters on the command line */
    length = read(STDIN_FILENO, inputBuffer, MAX_LINE); 

    start = -1;
    if (length == 0)
        exit(0);            /* ^d was entered, end of user command stream */
    if (length < 0){
        perror("error reading the command");
        exit(-1);           /* terminate with error code of -1 */
    }

    /* examine every character in the inputBuffer */
    for (i=0;i<length;i++) {
        switch (inputBuffer[i]){
          case ' ':
          case '\t' :               /* argument separators */
            if(start != -1){
                    args[ct] = &inputBuffer[start];    /* set up pointer */
                ct++;
            }
            inputBuffer[i] = '\0'; /* add a null char; make a C string */
            start = -1;
            break;
          case '\n':                 /* should be the final char examined */
            if (start != -1){
                    args[ct] = &inputBuffer[start];    
                ct++;
            }
                inputBuffer[i] = '\0';
                args[ct] = NULL; /* no more arguments to this command */
            break;
          default :             /* some other character */
            if (start == -1)
                start = i;
            if (inputBuffer[i] == '&'){
                *background  = 1;
                start = -1;
                inputBuffer[i] = '\0';
            }
          }
     }   
     args[ct] = NULL; /* just in case the input line was > 80 */
}

int builtIn_jobs() {
	
}

int builtIn_stop() {
	
}

int builtIn_bg() {

}

int builtIn_fg() {

}

int builtIn_kill() {

}

int getArgumentCeil(char** args) {
	int i;	
	for ( i = 0; args[i] != NULL; i++) { }
	return i;
}

int isBuiltIn(char* cmd, char* arg) {
	if (strcmp(cmd, "jobs")) {
		builtIn_jobs();
		return 1;
	}
	else if (strcmp(cmd, "stop")) {
		builtIn_stop();
		return 1;
	}
	else if (strcmp(cmd, "bg")) {
		builtIn_bg();
		return 1;
	}
	else if (strcmp(cmd, "fg")) {
		builtIn_fg();
		return 1;
	}
	else if (strcmp(cmd, "kill")) {
		builtIn_kill();
		return 1;
	}
	return 0;
}

int doForegroundCommand(char* cmd, char** args) {

}

int doBackgroundCommand(char* cmd, char** args) {

}

int isCommand(char* cmd, char** args) {
	int i;
	int numberOfArguments;
	int lengthOfFinalArgument;

	numberOfArguments = getArgumentCeil(args);
	for ( i = 0; (i > 0) && (args[numberOfArguments - 1][i] != '\0'); i++) { }
	lengthOfFinalArgument = i;

	if (lengthOfFinalArgument > 0) {
		if (args[numberOfArguments - 1][lengthOfFinalArgument - 1]
			       == '&') {
			/* here we're launching a background command 
			 * because & occured as the last character
			 * of the last argument*/	

			args[numberOfArguments - 1][lengthOfFinalArgument - 1] = '\0';
			return doBackgroundCommand(cmd, args);
		}
		int lastIndexOfCommand;
		for (lastIndexOfCommand = 0; 
				(cmd[lastIndexOfCommand] != '\0') &&
				(cmd[lastIndexOfCommand + 1] != '\0');
				 lastIndexOfCommand++) {}
		if (cmd[lastIndexOfCommand] == '&') {
			/* here we're launching as background
			 * because & was the last character
			 * of the command */
			cmd[lastIndexOfCommand] = '&';
			return doBackgroundCommand(cmd, args);
		}
	}
}

int doIntro() {
	printf("Welcome to kbshell. My pid is %d\n", getpid());
	return 0;
}

int doPrompt() {
	static int count = 0;
	static const char* initials = "jd";
	printf("%sshell[%d]: ", initials, count++);
	return 0;
}

int main(void)
{
char inputBuffer[MAX_LINE];      /* buffer to hold the command entered */
    int background;              /* equals 1 if a command is followed by '&' */
    char *args[(MAX_LINE/2)+1];  /* command line (of 80) has max of 40 arguments */

    doIntro(); 
    while (1){            /* Program terminates normally inside setup */
       background = 0;
       doPrompt();
       fflush(stdout);
       setup(inputBuffer,args,&background);       /* get next command */

      /* the steps are:
       (0) if built-in command, handle internally
       (1) if not, fork a child process using fork()
       (2) the child process will invoke execvp()
       (3) if background == 0, the parent will wait,
            otherwise returns to the setup() function. */

    }
}
