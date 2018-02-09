#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#define TRUE 1
#define FALSE 0

const char *student ="JUNYOUNG";

typedef struct{
	char *name;
	char *explain;
	int (*func)(int argc,char*argv[]);
}COMMAND;

pid_t pid;
int status;
int cmd_num=0;
char command[1000][100];
int cmd_cd(int argc,char*argv[]);
int cmd_history(int argc,char*argv[]);
int cmd_exit(int argc, char*argv[]);
int is_background(char *input);
void execute_command(char *input);

COMMAND builtin_cmds[]=
{
	{"cd", "change directory", cmd_cd},
	{"history","show command history",cmd_history},
	{"exit","exit shell", cmd_exit},
	{"quit","quit shell", cmd_exit},
};

int is_background(char *input)
{
	int i;
	for(i=0;i<strlen(input);i++)
	{
		if(input[i]=='&')
		{
			input[i]=' ';
			return TRUE;
		}
	}
	return FALSE;
}
int cmd_cd(int argc,char*argv[]){
	int i;
	if(strcmp("cd",argv[0])==0)
	{
		i=chdir(argv[1]);
		if(i!=0)
		{
			printf("No directory\n");
			return FALSE;
		}
		return TRUE;
	}
}

int cmd_history(int argc,char*argv[]){

	int i;
	for(i=0;i<cmd_num;i++)
	{
		printf("[%d] : %s\n",i+1,command[i]);
	}
	return TRUE;

}
int cmd_exit(int argc,char*argv[]){
	exit(0);
	return TRUE;
}

void execute_command(char *input)
{
	int i,j,k;
	int back=0,count=0;
	char *token[10];	
	char deli[]=" \t";
	back=is_background(input);	
	token[count++]=strtok(input,deli);
	while(token[count]!=NULL)
	{
		token[count++]=strtok(NULL,deli);
	}
	
	for(i=0;i<sizeof(builtin_cmds)/sizeof(COMMAND);i++)
	{
		if(strcmp(builtin_cmds[i].name,token[0])==0)
		{
			builtin_cmds[i].func(count,token);
			return;
		}
	}

	
	switch(pid=fork())
	{
	case -1:
		perror("[ERROR] : fork()");
	
	case 0:
		j=execvp(token[0],token);
		if(j==-1){
			printf("JYShell: %s:command not found\n",token[0]);
			exit(0);
		}
	default:
		if(back==TRUE)
		{
			printf("%d\n",getpid());
			back=0; 
			break;
		}
		waitpid(pid,&status,WUNTRACED);
		fflush(stdout);	
	}
}

int main(int argc,char*argv[])
{
	char input[1000];
	while(1)
	{	
		printf("%s$ [PATH:%s] $",student,get_current_dir_name());
		fgets(input,sizeof(input)/sizeof(char),stdin);
		input[strlen(input)-1]='\0';
		strcpy(command[cmd_num++],input);
		execute_command(input);
	}
	return 0;
}

