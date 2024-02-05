#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#define TOKEN_WORD_SIZE     15
#define COMMAND             tokens[0]
#define E_NOK               -1
#define E_OK                0
#define noDEBUG

typedef signed char STD_RET;

STD_RET get_command(char** input);
STD_RET parsing(char* command, char* tokens[],int* token_nums);
STD_RET myecho(char* tokens[]);
STD_RET mypwd();
STD_RET mycd(char* tokens[]);

int main()
{

   	char* cmd_line;
    	char** tokens;
    	int token_nums;
    	STD_RET ret;

    	while(1)
    	{
        	/*************** GET COMMAND LINE *************/
        	ret = get_command(&cmd_line);
        	if(ret != E_OK)
        	{
           		printf(">i can't get your command\n");
            		return E_NOK;
        	}
        
        	/************* PARSING COMMAND LINE ************/
        	tokens = (char**)malloc(sizeof(char*)); // Allocate memory for tokens
        	if (tokens == NULL) 
        	{
         		printf("Parsing failed\n");
         		return E_NOK;
        	}
        	ret = parsing(cmd_line, tokens,&token_nums);
        	if (ret != E_OK) 
        	{
            		printf("Parsing failed\n");
            		return E_NOK;
        	}
        	
        	/*************** empty command ***************/
        	if(COMMAND == NULL)
        	{
            		//nothing
        	}

        	/*************** myecho **********************/
        	else if(strcmp(COMMAND,"myecho") == 0)
        	{
        		myecho(tokens);
        	}

        	/*************** mypwd **********************/
        	else if(strcmp(COMMAND,"mypwd") == 0)
        	{
            		mypwd();
        	}

        	/*************** mycd **********************/
        	else if(strcmp(COMMAND,"mycd") == 0)
        	{
        	    	mycd(tokens);
        	}

        	/*************** EXIT **********************/
        	else if(strcmp(COMMAND,"exit") == 0)
        	{
			printf(">Goodbye\n");
            		return E_OK;
        	}

        	/*************** unknown command ***************/
        	else
        	{
            		printf(">this command isn't exist in my shell\n");
			
			pid_t returned_pid = fork();
			/********************* PARENT *********************/
			if (returned_pid > 0)
			{
				int wstatus;
				wait(&wstatus);
			}

			/*********************** CHILD *********************/
			else if (returned_pid == 0)
			{	
				char* new_program_argv[token_nums];
				for(int i=0; tokens[i] != NULL; ++i)
				{
					new_program_argv[i] = strdup(tokens[i]);
					new_program_argv[i+1] = NULL;
				}
				execvp(COMMAND, new_program_argv);

				/***************** bad child *****************/
				printf("bash-> couldn't find \"%s\"\n",COMMAND);
				return E_OK;
			}

			/******************** didn't Fork *******************/
			else
			{
				printf("busy to execute out of myshell\n");
			}
        	}

        	/************** FREE MEMORY *******************/
        	free(cmd_line);
            	free(tokens);
    	}
    	return E_OK;
}

STD_RET get_command(char** input)
{
    	// Allocate memory for the command line
    	*input =(char *)malloc(200 * sizeof(char));
    	if(NULL == *input)
    	{
        	return E_NOK;
    	}
    	else
    	{
		size_t n;
        	//get string from user
        	printf("\nyou -> ");
		int size = getline(input, &n, stdin);
        	
		// Remove the newline character if found
        	if ((*input)[strlen(*input) - 1] == '\n')
        	{
            		(*input)[strlen(*input) - 1] = '\0';
        	}
        	return E_OK;
    	}
}

STD_RET parsing(char* command, char** tokens, int* token_nums)
{
    int token_num = 0;
    char** dynamicArray = (char**)malloc(sizeof(char *));
    if (dynamicArray == NULL) {
        return E_NOK;
    }

    while((*command != '\0'))
    {
        //handle spaces
        if(*command == ' ')
        {
            command++;
        }
        else
        {
            //take new token
            int i=0;
            char buf[TOKEN_WORD_SIZE+1];
            while((*command != ' ' ) && (*command != '\0' ) && (i<TOKEN_WORD_SIZE))
            {
                buf[i] = *command++;
                i++;
            }
            buf[i]='\0';
            if(token_num == 0)
            {
                dynamicArray[0]=strdup(buf);
            }
            else
            {
                dynamicArray = (char**)realloc(dynamicArray, (token_num+1)*sizeof(char*));
                if (dynamicArray == NULL)
                {
                    return E_NOK;
                }
                dynamicArray[token_num] =strdup(buf);
            }
            token_num++;
        }
    }
    /***************** END DYNAMIC ARR BY NULL ******************************/
    dynamicArray = (char**)realloc(dynamicArray, (token_num+1)*sizeof(char*));
    if (dynamicArray == NULL)
    {
        return E_NOK;
    }
    dynamicArray[token_num] =NULL;
    for(int i=0; dynamicArray[i] != NULL ;++i)
    {
            tokens[i]=dynamicArray[i];
    }
    tokens[token_num] =NULL;
    *token_nums = token_num;
    free(dynamicArray);
    return E_OK;
}

STD_RET myecho(char* tokens[])
{
	char counter = 1;
	printf(">");
	while(tokens[counter] != NULL)
	{
		printf("%s ",tokens[counter]);
		counter++;
	}
	printf("\n");
}

STD_RET mypwd()
{
 	char buf[500]; //buf for the directory path

	//handle getcwd error
	if(getcwd(buf,sizeof(buf)) == NULL)
       	{
		printf(">can't get current directory\n");
	}	
	else
	{
        	printf(">Current directory : %s\n",buf);
	}
}

STD_RET mycd(char* tokens[])
{
        char cd_ret = chdir(tokens[1]);
	if(cd_ret != E_OK)
	{
		printf(">failed to chanage dir\n");
	}
}
