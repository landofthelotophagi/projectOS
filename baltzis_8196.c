#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <wait.h>
#include <signal.h>

int parse(char *line,char **argv){
	const char s[2] = ";";
	char *token;
	int counter=0;
	 
	/* get the first token - first command before semicolon */
	token = strtok(line, s);
	 
	/* walk through other tokens - one semicolon at a time */
	while( token != NULL ) {
	    argv[counter++]=token;
	    //printf( "%d: %s\n", counter,token ); //debugging
	    token = strtok(NULL, s);
	}
	argv[counter]='\0';
	 
	//printf("the children are: %d \n",counter);
	return counter;
}

void  Command(int i, char **argv)
{
	printf("The command of the child is: '%s' \n",argv[i]);  
	 
	char *line=argv[i];	 
	int index=0,temp=0;
    const char *p = "\t \r\n";
	char *token2;
	 
	/* get the first token */
	token2 = strtok(line, p);
	 
	/* walk through other tokens */
	if(token2 == NULL) temp=1;;
	while( token2 != NULL ) {
	    argv[index++]=token2;
	    token2 = strtok(NULL, p);
	}
	argv[index]='\0'; 
	 
	if(temp) argv[index]="\0"; // this allows us to accept white space betweem semicolons 	 
}

int  execute(char **argv,int children)
{
   	pid_t  child_pid,wpid;
   	int    status=0,temp=0;
	 
	for(int i=0; i<children; i++){ // fork a child process    	 
	    if ((child_pid = fork()) < 0) {
		 perror("fork");
		 exit(1);
	    }
	    else if (child_pid == 0) { // for the child process:         
	     //printf("In child process (pid = %d)\n", getpid());
		 Command(i,argv);
			 
		 if(strcmp(argv[0],"quit")==0 || strcmp(argv[0], "\0") == 0){
		     //do nothing 
		 }
		 else if (execvp(*argv, argv) < 0) { 
		     printf("ERROR: exec failed / command not found\n");
		     //perror(*argv);
		     exit(1);
		 }
		 exit(0);
	   }		 
	}
	 
    while((wpid = wait(&status)) > 0){ // wait for completion 
	    //printf("Exit status of %d was %d \n", (int)wpid, status);
		if (WIFEXITED(status) && WEXITSTATUS(status))  { //execvp() failed because no command is found or the command is misspelled										wrong so exit(1) and WEXITSTATUS(status)=1
			temp=1;  
		}
		else if(WIFEXITED(status)==0){
			printf("the child terminated abnormally \n");
		}
    }
	//printf("temp=%d \n",temp);
	return temp;
}

int ExitCheck(char *line){
	if(strstr(line, "quit") != 0)
	     return 1;
	else 
	     return 0;
}

void InteractiveMode(char **argv){
	char  line[512];             // the input line  
	int children,check,checkExecution;
	 
	while (1) {
		printf("baltzis_8196> ");   
		gets(line);  
		check=ExitCheck(line);
		printf("\n");
		children=parse(line,argv); //   parse the line and find the number of children           
		checkExecution=execute(argv,children);		// execute the command
		if(checkExecution) exit(1);
		if(check) exit(0); // if we have typed "quit" then we exit from the function
	}	 
}

void BatchMode(char **argv){
	char  line[512];             // the input line  
	int children,check,checkExecution;
	 
	FILE *fp;
	fp=fopen(argv[1],"r"); //open the file for reading
	if(fp==NULL){
		perror("Error opening file");
		exit(1);
	}
	if(fgets(line,512,fp)==NULL){
		printf("No command is found \n");
		exit(0);
	}
	fseek(fp, 0, SEEK_SET);
	while(fgets(line,512,fp)!= NULL){
		check=ExitCheck(line);
        children=parse(line,argv); //   parse the line and find the number of children 
		checkExecution=execute(argv,children);		// execute the command 
		if(checkExecution) exit(1);
		if(check) exit(0);	
     	}		 
}

int main(int argc, char **argv)
{  	 
	 if (argc==1) { //interactive mode
         	InteractiveMode(argv);
	 }
	 else if(argc==2){ //batch mode
	 	BatchMode(argv);
	 }
	 else{
	 	printf("Input arguments error\n");
	 }
	 
	 return 0;
} 
