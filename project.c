//COP4610
//Project 1 Starter Code
//example code for initial parsing

//*** if any problems are found with this code,
//*** please report them to the TA


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
typedef struct
{
    char** tokens;
    int numTokens;
} instruction;

void addToken(instruction* instr_ptr, char* tok);
void printTokens(instruction* instr_ptr);
void clearInstruction(instruction* instr_ptr);
void addNull(instruction* instr_ptr);
char* expandEnv(const char * name);
void inputAction(instruction* instr_ptr);
char* path(const char * name);
char * checkForPath(char *extra);
int fileExist(char * absolutePath);
void my_execute(char **cmd,int size);
int main() {
    char* token = NULL;
    char* temp = NULL;

    instruction instr;
    instr.tokens = NULL;
    instr.numTokens = 0;

    while (1) {
	char dir[100];
	 //if we use dir, we can save the directory in this variable for easier use when changing directories.    
	printf("%s@%s: %s> ", expandEnv("$USER"), expandEnv("$MACHINE"), getcwd(dir,100)); //<<  getcwd() saved the current directory to dir.
       // printf("%s@%s: %s> ", expandEnv("$USER"), expandEnv("$MACHINE"), expandEnv("$PWD"));

        // loop reads character sequences separated by whitespace
        do {
            //scans for next token and allocates token var to size of scanned token
            scanf("%ms", &token);
            temp = (char*)malloc((strlen(token) + 1) * sizeof(char));

            int i;
            int start = 0;
            for (i = 0; i < strlen(token); i++) {
                //pull out special characters and make them into a separate token in the instruction
                if (token[i] == '|' || token[i] == '>' || token[i] == '<' || token[i] == '&') {
                    if (i-start > 0) {
                        memcpy(temp, token + start, i - start);
                        temp[i-start] = '\0';
                        addToken(&instr, temp);
                    }

                    char specialChar[2];
                    specialChar[0] = token[i];
                    specialChar[1] = '\0';

                    addToken(&instr,specialChar);

                    start = i + 1;
                }
            }

            if (start < strlen(token)) {
                memcpy(temp, token + start, strlen(token) - start);
                temp[i-start] = '\0';
                addToken(&instr, temp);
            }

            //free and reset variables
            free(token);
            free(temp);

            token = NULL;
            temp = NULL;
        } while ('\n' != getchar());    //until end of line is reached
		addNull(&instr);
        inputAction(&instr);
		printTokens(&instr);
       
       // printTokens(&instr);                 // <----- 	COMMENTED OFF TOKEN PRINTING
        clearInstruction(&instr);

    }

    return 0;
}


// echo function now works and error checks for all possible environmental variables
// Renamed function
void inputAction(instruction* instr_ptr){
	int i;
	int check2Complete;
	char *check;
	char * recieve;
	char *check2;
	
	


		if (strcmp((instr_ptr->tokens)[0], "echo") == 0) {


			
			for (i = 1; i < instr_ptr->numTokens-1; i++) {
				if (((instr_ptr->tokens)[i][0]) == '$') {
					if (expandEnv((instr_ptr->tokens)[i]) == NULL) {
						printf("NO SUCH COMMAND EXISTS ");
					}
					else {
						printf("%s ", expandEnv((instr_ptr->tokens)[i]));
					}
				}
				else
					printf("%s ", (instr_ptr->tokens[i]));
			}

		}
		else if(strcmp((instr_ptr->tokens)[0],"cd") == 0){
			//we are missing 
			//1) if "cd" (no 2nd argument) then cd $HOME.
			//2)output "too many arguments if needed" 2 or more arguments
			//3)slides say to copy path to $PWD  using setenv?? helpful to do with parent and child running processes??
			if(chdir((instr_ptr->tokens)[1])!= 0 )
				perror((instr_ptr->tokens)[1]);
			//output "too many arguments if needed"
			//chdir("..");
		}
		else {
			for (i = 0; i < instr_ptr->numTokens; i++) {
				if ((instr_ptr->tokens)[i] != NULL) {

					check = strrchr(instr_ptr->tokens[i], '/');
					check2 = strrchr(instr_ptr->tokens[i], '-');
					check2Complete=check2 - instr_ptr->tokens[i];
					if (check == NULL) {
						if (check2Complete !=0) {
							recieve = checkForPath(instr_ptr->tokens[i]);
							//printf("%s%d", recieve, i);
						}
						else {
							recieve = instr_ptr->tokens[i];
							//printf("%s%d", recieve,i);
							

						}
						
					}
					else {
						recieve = path(instr_ptr->tokens[i]);
					//	printf("%s%d", recieve, i);

					}
					//if (strcmp(recieve, instr_ptr->tokens[i]) == 0) {
					//}
					if (recieve != NULL) {
						instr_ptr->tokens[i] = (char *)malloc((strlen(recieve) + 1) * sizeof(char));
						strcpy(instr_ptr->tokens[i], recieve);
						
					}
					else { break; }
				}

			}
			if (recieve != NULL) {
				my_execute(instr_ptr->tokens, instr_ptr->numTokens);
			}
			//printf("%s: NO SUCH COMMAND FOUND",(instr_ptr->tokens)[0]);
		}
	

    printf("\n");
}

char* path(const char * name) {
	int i;
	int fullString = 1;
	int catch22 = 0;
	int begining = 0;
	char *ptr;
	char *file;
	char *finisher;
	char **incompletePath;
	char *holder = (char*)malloc((strlen(name) + 1) * sizeof(char));
	for (i = 0; i < strlen(name) + 1; i++) {

		//pull out special characters and make them into a separate token in the instruction
		if (name[i] == '/' || name[i] == '\0') {

			i++;
			memcpy(holder, name + begining, i - begining);
			holder[i - begining] = '\0';

			if (catch22 == 0) {
				incompletePath = (char**)malloc(sizeof(char*));
			}
			else {
				incompletePath = (char**)realloc(incompletePath, (catch22 + 1) * sizeof(char*));
			}
			
			file = strrchr(holder, '.');
			if (strcmp(holder, "./") == 0) {
				
				incompletePath[catch22] = (char *)malloc((strlen(getenv("PWD")) + 2) * sizeof(char));
				strcpy(incompletePath[catch22], getenv("PWD"));
				strcat(incompletePath[catch22], "/");
			}
			else if (strcmp(holder, "../") == 0) {
				//	printf("%s\n", "2");

				char* pWD = getenv("PWD");
				ptr = strrchr(pWD, '/');

				*ptr = '\0';
				incompletePath[catch22] = (char *)malloc((strlen(pWD) + 2) * sizeof(char));
				strcpy(incompletePath[catch22], pWD);
				strcat(incompletePath[catch22], "/");
				*ptr = '/';
			}
			else if (file != NULL && name[i - 1] != '\0') {
				printf("%s\n", "there is a file where it is not suppose to be there");
				return;
			}
			else if ((strcmp(holder, "~/") == 0) && catch22 == 0) {
				//	printf("%s\n", "3");
				incompletePath[catch22] = (char *)malloc((strlen(getenv("HOME")) + 2) * sizeof(char));
				strcpy(incompletePath[catch22], getenv("HOME"));
				strcat(incompletePath[catch22], "/");
			}
			else if ((strcmp(holder, "/") == 0) && catch22 == 0) {
				//	printf("%s\n", "4");
				if (getenv("ROOT") != NULL) {
					incompletePath[catch22] = (char *)malloc((strlen(getenv("ROOT")) + 2) * sizeof(char));
					strcpy(incompletePath[catch22], getenv("ROOT"));

				}
				else {
					incompletePath[catch22] = (char *)malloc(2 * sizeof(char));
				}
				strcat(incompletePath[catch22], "/");
			}
			else {
				//printf("%s\n", "1");
				if (catch22 == 0) {
					incompletePath[catch22] = (char *)malloc((strlen(getenv("PWD")) + strlen(holder) + 2) * sizeof(char));
					strcpy(incompletePath[catch22], getenv("PWD"));
					strcat(incompletePath[catch22], "/");
					strcat(incompletePath[catch22], holder);
				}
				else {
					incompletePath[catch22] = (char *)malloc((strlen(holder) + 2) * sizeof(char));
					strcpy(incompletePath[catch22], holder);
				}

			}
			catch22++;
			begining = i;
		}


	}

	for (i = 0; i < catch22; i++) {
		if ((incompletePath)[i] != NULL)
			fullString += (strlen(incompletePath[i]));
		if (i == 0) {
			finisher = (char *)malloc((fullString) * sizeof(char));
			strcpy(finisher, incompletePath[i]);
		}
		else
		{
			finisher = (char *)realloc(finisher, (fullString) * sizeof(char));
			strcat(finisher, incompletePath[i]);
		}

	}
	if (fileExist(finisher) == 1) {
		return finisher;
	}
	printf("can't find file");
	return NULL;


}

char * checkForPath(char *extra) {
	int i;
	int begining = 0;
	int count = 0;
	char **incompletePath2;
	char * paths = getenv("PATH");
	char *holder2 = (char*)malloc((strlen(paths) + 1) * sizeof(char));
	for (i = 0; i < strlen(paths) + 1; i++) {
		if (paths[i] == ':' || paths[i] == '\0') {

			if (count == 0) {
				incompletePath2 = (char**)malloc(sizeof(char*));
			}
			else {
				incompletePath2 = (char**)realloc(incompletePath2, (count + 1) * sizeof(char*));
			}
			memcpy(holder2, paths + begining, i - begining);
			holder2[i - begining] = '\0';
			incompletePath2[count] = (char *)malloc((strlen(holder2) + strlen(extra) + 2) * sizeof(char));
			strcpy(incompletePath2[count], holder2);
			strcat(incompletePath2[count], "/");
			strcat(incompletePath2[count], extra);
			i++;

			begining = i;
			count++;
		}



	}
	/*	printf("%s\n", "file does not exist");
		char * pathTest;
		printf("%s\n", getenv("PATH"));
		for (i = 0; i < count; i++) {
			printf("%s\n",incompletePath2[i]);
		}*/

	for (i = 0; i < count; i++) {
		if (fileExist(incompletePath2[i]) == 1) {
			return incompletePath2[i];
		}
	}

	printf("%s\n", "file does not exist");
	return NULL;
}
int fileExist(char * absolutePath) {
	if (access(absolutePath, F_OK) == -1)
	{
		return 0;
	}
	return 1;
}
// Function to return any proper environmental variable value
char* expandEnv(const char * name) {

    char subbuff[strlen(name)];
    memcpy( subbuff, &name[1], strlen(name));
    subbuff[strlen(name)] = '\0';
    char * value = getenv(subbuff);
    return value;

}
void my_execute(char **cmd,int size) {

	int status;
	int i;
	for (i = 0; i < strlen(cmd); i++) {
		printf("%d", i);
	//	printf("%s", cmd[i]);
	}
	//cmd = (char**)realloc(cmd, (size + 1) * sizeof(char*));
	//cmd[size] = NULL;
	pid_t pid = fork();

	if (pid == -1) {

		//Error

		exit(1);
	}

	else if (pid == 0) {

		//Child

		execv(cmd[0], cmd);

		fprintf("Problem executing %s\n", cmd[0]);
		exit(1);

	}

	else {

		//Parent
		
		waitpid(pid, &status, 0);

	}
//	cmd = (char**)realloc(cmd, (size + 1) * sizeof(char*));
}

//reallocates instruction array to hold another token
//allocates for new token within instruction array
void addToken(instruction* instr_ptr, char* tok)
{
    //extend token array to accomodate an additional token
    if (instr_ptr->numTokens == 0)
        instr_ptr->tokens = (char**) malloc(sizeof(char*));
    else
        instr_ptr->tokens = (char**) realloc(instr_ptr->tokens, (instr_ptr->numTokens+1) * sizeof(char*));

    //allocate char array for new token in new slot
    instr_ptr->tokens[instr_ptr->numTokens] = (char *)malloc((strlen(tok)+1) * sizeof(char));
    strcpy(instr_ptr->tokens[instr_ptr->numTokens], tok);

    instr_ptr->numTokens++;
}

void addNull(instruction* instr_ptr)
{
    //extend token array to accomodate an additional token
    if (instr_ptr->numTokens == 0)
        instr_ptr->tokens = (char**)malloc(sizeof(char*));
    else
        instr_ptr->tokens = (char**)realloc(instr_ptr->tokens, (instr_ptr->numTokens+1) * sizeof(char*));

    instr_ptr->tokens[instr_ptr->numTokens] = (char*) NULL;
    instr_ptr->numTokens++;
}

void printTokens(instruction* instr_ptr)
{
    int i;
    printf("Tokens:\n");
    for (i = 0; i < instr_ptr->numTokens; i++) {
        if ((instr_ptr->tokens)[i] != NULL)
            printf("%s\n", (instr_ptr->tokens)[i]);
    }
}

void clearInstruction(instruction* instr_ptr) {
    int i;
    for (i = 0; i < instr_ptr->numTokens; i++)
        free(instr_ptr->tokens[i]);

    free(instr_ptr->tokens);

    instr_ptr->tokens = NULL;
    instr_ptr->numTokens = 0;
}
