#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

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
	for ( i = 0; args[i] != NULL;) { i++; }
	return i;
}

int isBuiltIn(char* cmd, char** arg) {
	if (strcmp(cmd, "jobs") == 0 ) {
		builtIn_jobs();
		printf("executing builtin: jobs\n");
		return 0;
	}
	else if (strcmp(cmd, "stop") == 0 ) {
		builtIn_stop();
		printf("executing builtin: stop\n");
		return 0;
	}
	else if (strcmp(cmd, "bg") == 0 ) {
		builtIn_bg();
		printf("executing builtin: bg\n");
		return 0;
	}
	else if (strcmp(cmd, "fg") == 0 ) {
		builtIn_fg();
		printf("executing builtin: fg\n");
		return 0;
	}
	else if (strcmp(cmd, "kill") == 0 ) {
		builtIn_kill();
		printf("executing builtin: kill\n");
		return 0;
	}
	return -1;
}


int doHandleChildProcessEnded(int childPID, int* childStatus) {
	if (childPID == -1) {
		return -1;
	} else if (WIFEXITED(*childStatus)) {
		printf("%d exited, status %d\n", childPID, WEXITSTATUS(*childStatus));
	} else if (WIFSIGNALED(*childStatus)) {
		printf("%d killed by signal  %d\n", childPID, WTERMSIG(*childStatus));
	} else if (WIFSTOPPED(*childStatus)) {
		printf("%d stopped by signal %d\n", childPID, WSTOPSIG(*childStatus));
	} else if (WIFCONTINUED(*childStatus)) {
		printf("%d continued\n", childPID);
	}
}

int BACKGROUND_PROCESS_CEIL = 50;
int PIDsOfBackgroundProcesses[50];
int StatusOfBackgroundProcesses[50];
/* if this is zero, we have no foreground process */
int PIDOfForegroundProcess = 0;
static int CommandCount = 0;

int getBackgroundProcessIndex(int pid) {
	int i;
	for ( i = 0; i < 50; i++ ){
		if (PIDsOfBackgroundProcesses[i] == pid)
			return i;	
	}
}

void setBackgroundProcessAs(int pid, int stats) {
	StatusOfBackgroundProcesses[getBackgroundProcessIndex(pid)] = stats;
}

int getBackgroundProcessStatus(int pid) {	
	return StatusOfBackgroundProcesses[getBackgroundProcessIndex(pid)];
}

int doForegroundCommand(char* cmd, char** args) {
	pid_t child_pid;
	int child_status;
	
	static int count = 0;
	PIDsOfBackgroundProcesses[CommandCount] = PIDOfForegroundProcess = child_pid = fork();
	StatusOfBackgroundProcesses[CommandCount++] = 1;
	if (child_pid == 0) {
		/* continue as child process */
		execvp(cmd, args);

		/* continues only if execvp fails */
		printf("failed to invoke %s\n", cmd);
		return -1;
	} else {
		printf("[Child pid = %d, background = %s]\n", child_pid, "FALSE");
		/* as it is a foreground command, we'll wait
		 * here until it finished */
		waitpid(child_pid, &child_status, WUNTRACED | WCONTINUED);
		PIDOfForegroundProcess = 0;
		setBackgroundProcessAs(child_pid, 0);
		doHandleChildProcessEnded(child_pid, &child_status);
	}
	return 0;
}

int doBackgroundCommand(char* cmd, char** args) {
	pid_t child_pid;
	int child_status;
	
	PIDsOfBackgroundProcesses[CommandCount] = child_pid = fork();
	StatusOfBackgroundProcesses[CommandCount++] = 1;
	if (child_pid == 0) {
		printf("[Child pid = %d, background = %s]\n", child_pid, "TRUE");

		/* continue as child process */
		execvp(cmd, args);

		/* continues only if execvp fails */
		printf("failed to invoke %s\n", cmd);
		return -1;
	}
}

int isCommand(char* cmd, char** args) {
	if (cmd[0] == '\0') return 0;

	int i;
	int argumentCeil;
	int lengthOfFinalArgument;

	argumentCeil = getArgumentCeil(args);
	for ( i = 0; args[argumentCeil - 1][i] == '\0';) { i++; } 
	lengthOfFinalArgument = i;

	if (args[argumentCeil - 1][lengthOfFinalArgument - 1] == '&') {
		if (lengthOfFinalArgument == 1) {
			if (argumentCeil == 1) {
				return 0;
			}
			args[argumentCeil - 1] = NULL;
			argumentCeil--;
			return doBackgroundCommand(cmd, args);
		}
	}

	if (lengthOfFinalArgument > 0) {
		if (args[numberOfArguments - 1][lengthOfFinalArgument - 1]
			       == '&') {
			/* here we're launching a background command 
			 * because & occured as the last character
			 * of the last argument*/	

			args[numberOfArguments - 1][lengthOfFinalArgument - 1] = '\0';
			if ((lengthOfFinalArgument - 1) == 0)
				args[numberOfArguments - 1] = NULL;
		}
	
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
		cmd[lastIndexOfCommand] = '\0';
		return doBackgroundCommand(cmd, args);
	}
	else {
		doForegroundCommand(cmd, args);
	}

}

void handle_SIGQUIT() {
	/*write(STDOUT_FILENO, buffer, strlen(buffer));
	  */
}

int doWait(int* status) {
	return wait(status);
}

int doIntro() {
	printf("Welcome to kbshell. My pid is %d\n", getpid());
	return 0;
}

int doPrompt() {
	static const char* initials = "jd";
	printf("%sshell[%d]: ", initials, CommandCount++);
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
       int status;
       int pid_finished = doWait(&status);
       doHandleChildProcessEnded(pid_finished, &status);
       doPrompt();
       fflush(stdout);
       setup(inputBuffer,args,&background);       /* get next command */

       if (isBuiltIn(inputBuffer, args) == -1) {
		if (isCommand(inputBuffer, args) == -1) {
			printf("failed to find %s\n", inputBuffer);
		}
       }

      /* the steps are:
       (0) if built-in command, handle internally
       (1) if not, fork a child process using fork()
       (2) the child process will invoke execvp()
       (3) if background == 0, the parent will wait,
            otherwise returns to the setup() function. */

    }
}
