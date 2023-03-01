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

/* setup kept as is */

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

/* give ourselves some space to store the pids of jobs we issue and their status */
int BACKGROUND_PROCESS_CEIL = 50;
int PIDsOfBackgroundProcesses[50];
int StatusOfBackgroundProcesses[50];
char StatusOfBackgroundProcessesString[50][50];
/* if this is zero, we have no foreground process */
static int CommandCount = 1;

/* these three functions allow for us to go between job numbers and pids */
int getBackgroundProcessIndex(int pid) {
	int i;
	for ( i = 1; i < 50; i++ ){
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

int printDetailsAboutBackgroundProcess(int i) {
	if (i != 0) {
		if (PIDsOfBackgroundProcesses[i] != -1 )
			printf("[%d] (PID: %d): ", i, PIDsOfBackgroundProcesses[i]);
		/*if (WIFEXITED(StatusOfBackgroundProcesses[i]))
			printf("exited %d ", WEXITSTATUS(StatusOfBackgroundProcesses[i]));
		if (WIFSIGNALED(StatusOfBackgroundProcesses[i]))
			printf("signaled ");
		if (WTERMSIG(StatusOfBackgroundProcesses[i]))
			printf("terminated ");
		if (WIFSTOPPED(StatusOfBackgroundProcesses[i]))
			printf("stopped %d ", WSTOPSIG(StatusOfBackgroundProcesses[i]));
		if (WIFCONTINUED(StatusOfBackgroundProcesses[i]))
			printf("contiued ");*/
		printf("%s", StatusOfBackgroundProcessesString[i]);
		printf("\n");
	}
}

/* look at the last saved status of each command and try to reveal
 * as much information as possible as is comtained in sttaus */
int builtIn_jobs() {
	int i = 0;
	for ( i = 1; i < CommandCount; i++) {
		printDetailsAboutBackgroundProcess(i);
	}
}

/* these four functions are are different in the signal they send */
int builtIn_stop(int pid) {
	kill(pid, SIGSTOP);
	int index = getBackgroundProcessIndex(pid);
	printf("stopped job %d\n", index);
	/* attempt to incorporate new information in to status */
	StatusOfBackgroundProcesses[index] = WSTOPPED;
	strcpy(StatusOfBackgroundProcessesString[index], "Stopped");
	printDetailsAboutBackgroundProcess(index);
	return 0;
}

int builtIn_bg(int pid) {
	kill(pid, SIGCONT);
	int index = getBackgroundProcessIndex(pid);
	printf("backgrounding job %d\n", index);
	StatusOfBackgroundProcesses[index] =  WCONTINUED;
	strcpy(StatusOfBackgroundProcessesString[index], "Continued");
	printDetailsAboutBackgroundProcess(index);
	/* we're not waiting around unlike bg, return */
	return 0;
}


int builtIn_fg(int pid) {
	kill(pid, SIGCONT);
	int index = getBackgroundProcessIndex(pid);
	printf("foregrounding job %d\n", index);
	StatusOfBackgroundProcesses[index] = WCONTINUED;
	strcpy(StatusOfBackgroundProcessesString[index], "Continued");
	printDetailsAboutBackgroundProcess(index);
	int status;
	/* the only difference with bg and fg is theat here we wait */
	return waitpid(pid, &status, WUNTRACED);
}

int builtIn_kill(int pid) {
	kill(pid, SIGKILL);
	int index = getBackgroundProcessIndex(pid);
	printf("killing job %d\n", index);
	StatusOfBackgroundProcesses[index] = WEXITED;
	strcpy(StatusOfBackgroundProcessesString[index], "Exited");
	printDetailsAboutBackgroundProcess(index);
	return 0;
}

int builtIn_ps(int pid) {
	int index = getBackgroundProcessIndex(pid);
	return printDetailsAboutBackgroundProcess(index);
}

/* simply returns +1 on this highest index */
int getArgumentCeil(char** args) {
	int i;	
	for ( i = 0; args[i] != NULL;) { i++; }
	return i;
}

/* differentiates between the various background commands, if noone match
 * returns a value to indicate to issue as a command */
int isBuiltIn(char* cmd, char** arg) {
	if (strcmp(cmd, "jobs") == 0 ) {
		builtIn_jobs();
		printf("executing builtin: jobs\n");
		return 0;
	}
	else if (strcmp(cmd, "stop") == 0 ) {
		int pid = atoi (arg[1]);
		builtIn_stop(pid);
		printf("executing builtin: stop on %d\n", pid);
		return 0;
	}
	else if (strcmp(cmd, "bg") == 0 ) {
		int pid = atoi (arg[1]);
		builtIn_bg(pid);
		printf("executing builtin: bg on %d\n", pid);
		return 0;
	}
	else if (strcmp(cmd, "fg") == 0 ) {
		int pid = atoi (arg[1]);
		builtIn_fg(pid);
		printf("executing builtin: fg on %d\n", pid);
		return 0;
	}
	else if (strcmp(cmd, "kill") == 0 ) {
		int pid = atoi (arg[1]);
		builtIn_kill(pid);
		printf("executing builtin: kill on %d\n", pid);
		return 0;
	}
	else if (strcmp(cmd, "ps") == 0 ) {
		int pid = atoi (arg[1]);
		builtIn_ps(pid);
		printf("executing builtin: ps on %d\n", pid);
		return 0;
	}
	else if (strcmp(cmd, "exit") == 0 ) {
		printf("jdshell exiting\n");
		exit(0);
	}
	PIDsOfBackgroundProcesses[CommandCount - 1] = -1;
	return -1;
}

/* we come here whenever we went and waited for a process to end */
int doHandleChildProcessEnded(int childPID, int* childStatus) {
	if (childPID == -1) {
		return -1;
	}
	int index = getBackgroundProcessIndex(childPID);	
	if (WIFEXITED(*childStatus)) {
		printf("%d exited, status %d\n", childPID, WEXITSTATUS(*childStatus));
		strcpy(StatusOfBackgroundProcessesString[index], "Exited");
	} else if (WIFSIGNALED(*childStatus)) {
		printf("%d killed by signal  %d\n", childPID, WTERMSIG(*childStatus));
		strcpy(StatusOfBackgroundProcessesString[index], "Killed");
	} else if (WIFSTOPPED(*childStatus)) {
		printf("%d stopped by signal %d\n", childPID, WSTOPSIG(*childStatus));
		strcpy(StatusOfBackgroundProcessesString[index], "Stopped");
	} else if (WIFCONTINUED(*childStatus)) {
		printf("%d continued\n", childPID);
		strcpy(StatusOfBackgroundProcessesString[index], "Continued");
	}
	return 0;
}


int doForegroundCommand(char* cmd, char** args) {
	pid_t child_pid;
	int child_status;
	
	static int count = 0;
	child_pid = fork();
	if (child_pid == 0) {
		/* continue as child process */
		execvp(cmd, args);

		/* continues only if execvp fails */
		printf("failed to invoke %s\n", cmd);
		return -1;
	} else {
		PIDsOfBackgroundProcesses[CommandCount - 1] = child_pid;
		printf("[Child pid = %d, background = %s]\n", child_pid, "FALSE");
		/* as it is a foreground command, we'll wait
		 * here until it finished */
		waitpid(child_pid, &child_status, WUNTRACED | WCONTINUED);
		setBackgroundProcessAs(child_pid, child_status);
		doHandleChildProcessEnded(child_pid, &child_status);
	}
	return 0;
}

int doBackgroundCommand(char* cmd, char** args) {
	pid_t child_pid;
	int child_status;
	
	child_pid = fork();
	if (child_pid == 0) {
		/* continue as child process */
		execvp(cmd, args);

		/* continues only if execvp fails */
		printf("failed to invoke %s\n", cmd);
		return -1;
	}
	else {
		/* insde here we're saving details on the process and returning 
		 * so that the shell can be issued another */
		PIDsOfBackgroundProcesses[CommandCount - 1] = child_pid;
		setBackgroundProcessAs(child_pid, WCONTINUED);
		printf("[Child pid = %d, background = %s]\n", child_pid, "TRUE");
		return 0;
	}
}

/* basic branch between the two remaining possibilities */
int isCommand(char* cmd, char** args, int isBackground) {
	if (isBackground) {
		return doBackgroundCommand(cmd, args);		
	}
	else {
		return doForegroundCommand(cmd, args);
	}
}


int sigHandlerBufferSize = 50;
char sigHandlerBuffer[50];

/* handle a quit, pass it oh, handle errors */
void handle_SIGQUIT() {
	/*write(STDOUT_FILENO, buffer, strlen(buffer));	  */
	printf("Caught Ctrl-\\\n");
	if (kill(PIDsOfBackgroundProcesses[CommandCount - 1], SIGQUIT) != -1)
		printf("Quitting %d\n", PIDsOfBackgroundProcesses[CommandCount - 1]);
	else
		printf("Failed to quit %d\n", PIDsOfBackgroundProcesses[CommandCount - 1]);
}

/* swapped in the new waitid so I could nohang */
int doWait(int* status) {
	siginfo_t t;
	int ret = waitid(P_PID, getpid(), &t, WNOHANG);
	*status = t.si_code;
	return ret;
}

/* first time through only */
int doIntro() {
	printf("Welcome to kbshell. My pid is %d\n", getpid());
	return 0;
}

/* first time and every other time */
int doPrompt() {
	static const char* initials = "jd";
	printf("%sshell[%d]: ", initials, CommandCount++);
	return 0;
}

int main(void)
{
char inputBuffer[MAX_LINE];      /* buffer to hold the command entered */
    int background = 0;              /* equals 1 if a command is followed by '&' */
    char *args[(MAX_LINE/2)+1];  /* command line (of 80) has max of 40 arguments */
	
	 
	struct sigaction handler;
	handler.sa_handler = handle_SIGQUIT;
	sigaction(SIGQUIT, &handler, NULL);

    doIntro(); 
    while (1){            /* Program terminates normally inside setup */
       int status;
       int pid_finished;
       if (!background) {
		int pid_finished = doWait(&status);
		background = 0;
		doHandleChildProcessEnded(pid_finished, &status);
       } 
       sleep(1);
       background = 0;
       doPrompt();
       fflush(stdout);
       setup(inputBuffer,args,&background);       /* get next command */

       if (inputBuffer[0] != '\0') {
	       if (isBuiltIn(inputBuffer, args) == -1) {
			if (isCommand(inputBuffer, args, background) == -1) {
				printf("failed to find %s\n", inputBuffer);
			}
		}
       }
    }

      /* the steps are:
       (0) if built-in command, handle internally
       (1) if not, fork a child process using fork()
       (2) the child process will invoke execvp()
       (3) if background == 0, the parent will wait,
            otherwise returns to the setup() function. */

}
