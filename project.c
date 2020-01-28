//COP4610
//Project 1 Starter Code
//example code for initial parsing

//*** if any problems are found with this code,
//*** please report them to the TA


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
typedef struct
{
	char** tokens;
	int numTokens;
} instruction;

// Following code creates a basic queue structure
// A structure to represent a queue
typedef struct
{
	int PIQ, size;
	pid_t PID;
	char** command;
} RBP;
struct Queue
{
	int front, rear, size, bcounter;
	unsigned capacity;
	RBP* array;
};

// function to create a queue of given capacity.
// It initializes size of queue as 0
struct Queue* createQueue(unsigned capacity)
{
	struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
	queue->capacity = capacity;
	queue->bcounter = 0;
	queue->front = queue->size = 0;
	queue->rear = capacity - 1;  // This is important, see the enqueue
	queue->array = (RBP*)malloc(queue->capacity * sizeof(RBP));
	return queue;
}

// Queue is full when size becomes equal to the capacity
int isFull(struct Queue* queue)
{
	return (queue->size == queue->capacity);
}

// Queue is empty when size is 0
int isEmpty(struct Queue* queue)
{
	return (queue->size == 0);
}

// Function to add an item to the queue.
// It changes rear and size
void enqueue(struct Queue* queue, RBP item)
{
	if (isFull(queue))
		return;
	queue->rear = (queue->rear + 1) % queue->capacity;
	queue->array[queue->rear] = item;
	queue->size = queue->size + 1;
	//printf("%d enqueued to queue\n", item);
}

// Function to remove an item from queue.
// It changes front and size
RBP dequeue(struct Queue* queue)
{
	if (isEmpty(queue)) {
		printf("Mistaken dequeue call. Exiting program.");
		exit(1);
	}
	RBP item = queue->array[queue->front];
	queue->front = (queue->front + 1) % queue->capacity;
	queue->size = queue->size - 1;
	return item;
}

// Function to get front of queue
RBP front(struct Queue* queue)
{
	if (isEmpty(queue)) {
		printf("Mistaken dequeue call. Exiting program.");
		exit(1);
	}
	return queue->array[queue->front];
}

// Function to get rear of queue
RBP rear(struct Queue* queue)
{
	if (isEmpty(queue)) {
		printf("Mistaken dequeue call. Exiting program.");
		exit(1);
	}
	return queue->array[queue->rear];
}

void addToken(instruction* instr_ptr, char* tok);
void printTokens(instruction* instr_ptr);
void clearInstruction(instruction* instr_ptr);
void addNull(instruction* instr_ptr);
char* expandEnv(const char * name);
void inputAction(instruction* instr_ptr, struct Queue* queue);
char* path(const char * name, int pass);
char * checkForPath(char *extra);
int fileExist(char * absolutePath);
void my_execute(char **cmd, int size, struct Queue* queue, int bcheck);

int main() {
	char* token = NULL;
	char* temp = NULL;

	instruction instr;
	instr.tokens = NULL;
	instr.numTokens = 0;

	struct Queue* cmdqueue = createQueue(100);

	while (1) {

		if (isEmpty(cmdqueue) == 0) {
      int tempint, status, i;
      for (tempint = 0; tempint < cmdqueue->bcounter; tempint++) {
        	if (waitpid(cmdqueue->array[tempint].PID, &status, WNOHANG) != 0) {}
					else {
          	printf("[%d]+    [", cmdqueue->array[tempint].PIQ);
						//cmdqueue->array[tempint].command[0]);
          	dequeue(cmdqueue);
          }
			}
    }

		printf("%s@%s: %s> ", expandEnv("$USER"), expandEnv("$MACHINE"), expandEnv("$PWD"));

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
					if (i - start > 0) {
						memcpy(temp, token + start, i - start);
						temp[i - start] = '\0';
						addToken(&instr, temp);
					}

					char specialChar[2];
					specialChar[0] = token[i];
					specialChar[1] = '\0';

					addToken(&instr, specialChar);

					start = i + 1;
				}
			}

			if (start < strlen(token)) {
				memcpy(temp, token + start, strlen(token) - start);
				temp[i - start] = '\0';
				addToken(&instr, temp);
			}

			//free and reset variables
			free(token);
			free(temp);

			token = NULL;
			temp = NULL;
		} while ('\n' != getchar());    //until end of line is reached
		addNull(&instr);
		inputAction(&instr, cmdqueue);
		printTokens(&instr);
		clearInstruction(&instr);

		// Testing to see if queue works
		// printf("%d dequeued from queue\n\n", dequeue(cmdqueue));
	}

	return 0;
}


// echo function now works and error checks for all possible environmental variables
// Renamed function
void inputAction(instruction* instr_ptr, struct Queue* queue) {
	int i, syncheck;
	int check2Complete;
	char *check;
	char *recieve;
	char *check2;
	char *check3;
	char *check4;
	char *check5;
	char dir[100];
	getcwd(dir, 100);

	// Ignores leading '&' if it occurs
	if ((strcmp((instr_ptr->tokens)[i], "&") == 0) && (i == 0)) {
		//printf("Before Allocation:\n");
		//printTokens(instr_ptr);
		//printf("CURRENT TOKEN COUNT BEFORE ALLOCATION: %d\n", (sizeof(instr_ptr->tokens)) / (sizeof(char *)));
		char **tempArray = malloc((instr_ptr->numTokens - 1) * sizeof(char *));
		for (i = 1; i < instr_ptr->numTokens - 1; i++) {
			(tempArray)[i - 1] = (instr_ptr->tokens)[i];
		}
		free(instr_ptr->tokens);
		instr_ptr->tokens = tempArray;
		//printf("Number of Tokens 1: %d\n", instr_ptr->numTokens);
		instr_ptr->numTokens = instr_ptr->numTokens - 1;
		//printf("Number of Tokens 2: %d\n", instr_ptr->numTokens);
		//printf("After Allocation:\n");
		//printTokens(instr_ptr);
		//printf("TOKEN COUNT AFTER ALLOCATION: %d\n", (sizeof(instr_ptr->tokens)) / (sizeof(char *)));
	}

	for (i = 0; i < instr_ptr->numTokens - 1; i++)
	{
		if ((strcmp((instr_ptr->tokens)[i], "&") == 0) && (i == instr_ptr->numTokens - 2)) {

			syncheck = 1;

			break;
			// Background process needs to be initialized here
		}
		else if (strcmp((instr_ptr->tokens)[i], "&") == 0) {
			syncheck = 2;
			printf("INVALID SYNTAX\n");
			break;
			// Invalid Syntax
		}
		else {
			syncheck = 0;
			// Non-Background Process
			// printf("TOKEN COUNT NO ALLOCATION: %d\n", (sizeof(instr_ptr->tokens)) / (sizeof(char *)));
			//printf("No Allocation:\n");
			//printTokens(instr_ptr);
		}
	}


	if (syncheck == 0 || syncheck == 1) {

		if (syncheck == 1) {
			//printf("Before Allocation:\n");
			//printTokens(instr_ptr);
			//printf("CURRENT TOKEN COUNT BEFORE ALLOCATION: %d\n", (sizeof(instr_ptr->tokens)) / (sizeof(char *)));
			char **tempArray = malloc((instr_ptr->numTokens - 2) * sizeof(char *));
			for (i = 0; i < (instr_ptr->numTokens - 2); i++) {
				(tempArray)[i] = (instr_ptr->tokens)[i];
			}
			(tempArray)[instr_ptr->numTokens - 2] = NULL;
			free(instr_ptr->tokens);
			instr_ptr->tokens = tempArray;
			//printf("Number of Tokens 1: %d\n", instr_ptr->numTokens);
			instr_ptr->numTokens = instr_ptr->numTokens - 1;
			//printf("After Allocation:\n");
			//printf("Number of Tokens 2: %d\n", instr_ptr->numTokens);
			//printTokens(instr_ptr);
			//printf("TOKEN COUNT AFTER ALLOCATION: %d\n", (sizeof(instr_ptr->tokens)) / (sizeof(char *)));
		}

		if (strcmp((instr_ptr->tokens)[0], "echo") == 0) {
			for (i = 1; i < instr_ptr->numTokens - 1; i++) {
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
		else if (strcmp((instr_ptr->tokens)[0], "cd") == 0) {
			if ((instr_ptr->tokens)[2] == NULL) {
				if ((instr_ptr->tokens)[1] == NULL) {
					chdir(getenv("HOME"));
					char* temp;
					temp = getenv("HOME");
					setenv("PWD", temp, 1);
				}
				else {
					if (chdir((instr_ptr->tokens)[1]) != 0)
						perror((instr_ptr->tokens)[1]);
					else {
						setenv("PWD", getcwd(dir, 100), 1);
					}
				}
			}
			else
				printf("Too many arguments\n");
		}
		//pipe command found in user input
		else if ((instr_ptr->tokens)[1] != NULL && strcmp((instr_ptr->tokens)[1], "|") == 0 || (instr_ptr->tokens)[2] != NULL && strcmp((instr_ptr->tokens)[2], "|") == 0) {
			//syntax error check if user does not input 2 arguments with the pipe.
			if ((instr_ptr->tokens)[2] == NULL) {
				printf("ERROR: invalid syntax, no 2nd argument found\n");
			}
			else {
				printf("pipe needs to happen here\n");
			}
		}
		else {
			for (i = 0; i < instr_ptr->numTokens; i++) {
				if ((instr_ptr->tokens)[i] != NULL) {

					check = strrchr(instr_ptr->tokens[i], '/');
					check2 = strrchr(instr_ptr->tokens[i], '-');
					check3 = strrchr(instr_ptr->tokens[i], '>');
					check4 = strrchr(instr_ptr->tokens[i], '<');
					check2Complete = check2 - instr_ptr->tokens[i];
					if (check3 != NULL) {
						recieve = NULL;

						if (i + 5 == instr_ptr->numTokens) {

							check5 = strrchr(instr_ptr->tokens[i + 2], '<');
							if (check5 != NULL) {

								if (i == 0) {
									printf("the greater then or less then sign canot be at the start of the input");
								}
								else {
									FILE *target = fopen(path(instr_ptr->tokens[i + 3], 0), "r");
									FILE * gate = fopen(path(instr_ptr->tokens[i + 1], 1), "w");
									int fd = fileno(target);
									int fc = fileno(gate);
									instr_ptr->tokens[i] = NULL;
									if (target != NULL) {
										if (fork() == 0) {//Child
											close(STDIN_FILENO);
											dup(fd);
											close(STDOUT_FILENO);
											dup(fc);
											printf("targethit");
											my_execute(instr_ptr->tokens, i, queue, syncheck);
											close(fd);//Executeprocess
											close(fc);//Executeprocess
										}
										else {//Parent
											close(fc);
											close(fd);
										}
									}

								}
							}
							else {
								printf("there seems to ba file after after input");

							}
						}
						else if (i == 0) {
							printf("the greater then or less then sign cannot be at the start of the input");
						}
						else if (i + 2 == instr_ptr->numTokens) {
							printf("the greater then or less then sign cannot be at the end of the input");
						}
						else if (i + 3 == instr_ptr->numTokens) {
							FILE * gate = fopen(path(instr_ptr->tokens[i + 1], 1), "w");
							int fd = fileno(gate);
							instr_ptr->tokens[i] = NULL;
							if (fork() == 0) {
								close(STDOUT_FILENO);
								dup(fd);
								my_execute(instr_ptr->tokens, i, queue, syncheck);
								close(fd);

							}
							else {
								close(fd);
							}
						}

						else {
							printf("%d%d", i, instr_ptr->numTokens);
							printf("there seems to ba file after after input");
						}

						break;
					}
					else if (check4 != NULL) {
						recieve = NULL;

						if (i + 5 == instr_ptr->numTokens) {
							check5 = strrchr(instr_ptr->tokens[i + 2], '>');
							if (check5 != NULL) {
								if (i == 0) {
									printf("the greater then or less then sign canot be at the start of the input");
								}
								else {
									FILE *target = fopen(path(instr_ptr->tokens[i + 1], 0), "r");
									FILE * gate = fopen(path(instr_ptr->tokens[i + 3], 1), "w");
									int fd = fileno(target);
									int fc = fileno(gate);
									instr_ptr->tokens[i] = NULL;
									if (target != NULL) {
										if (fork() == 0) {//Child
											close(STDIN_FILENO);
											dup(fd);
											close(STDOUT_FILENO);
											dup(fc);

											my_execute(instr_ptr->tokens, i, queue, syncheck);
											close(fd);//Executeprocess
											close(fc);//Executeprocess
										}
										else {//Parent
											close(fc);
											close(fd);
										}
									}

								}
							}
							else {
								printf("there seems to ba file after after input");

							}
						}
						else if (i == 0) {
							printf("the greater then or less then sign canot be at the start of the input");
						}
						else if (i + 2 == instr_ptr->numTokens) {
							printf("the greater then or less then sign canot be at the end of the input");
						}
						else if (i + 3 == instr_ptr->numTokens) {
							FILE * gate = fopen(path(instr_ptr->tokens[i + 1], 0), "r");
							if (gate != NULL) {
								int fd = fileno(gate);
								instr_ptr->tokens[i] = NULL;
								if (fork() == 0) {//Child
									close(STDIN_FILENO);
									dup(fd);


									my_execute(instr_ptr->tokens, i, queue, syncheck);
									close(fd);//Executeprocess

								}
								else {//Parent
									close(fd);
								}
							}
							else {
								printf("this file does not exist");

							}
						}
						else {
							// printf("there seems to ba file after after input");
						}
						break;
					}
					else if (check == NULL) {
						if (check2Complete != 0) {
							recieve = checkForPath(instr_ptr->tokens[i]);
							//printf("%s%d", recieve, i);
						}
						else {
							recieve = instr_ptr->tokens[i];
						}
					}
					else {
						recieve = path(instr_ptr->tokens[i], 0);
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
				my_execute(instr_ptr->tokens, instr_ptr->numTokens, queue, syncheck);
			}
			//printf("%s: NO SUCH COMMAND FOUND",(instr_ptr->tokens)[0]);
		}
	}
	else if (syncheck == 2) {
		printf("Invalid Syntax: The '&' character can only be at the beginning\nor end of a command\n");
	}
	else {
		printf("'&' ERROR CHECKING ISSUE\n");
		exit(1);
	}
	printf("\n");
}

char* path(const char * name, int pass) {
	int i;
	int fullString = 1;
	int catch22 = 0;
	int begining = 0;
	char *ptr;
	char *file;
	char *finisher;
	char **incompletePath;
	printf("%d\n", name);
	char *holder = (char*)malloc((strlen(name) + 1) * sizeof(char));
	printf("%s\n", "11");
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
				printf("%s\n", "8");
				incompletePath[catch22] = (char *)malloc((strlen(getenv("PWD")) + 2) * sizeof(char));
				strcpy(incompletePath[catch22], getenv("PWD"));
				strcat(incompletePath[catch22], "/");
			}
			else if (strcmp(holder, "../") == 0) {


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

				incompletePath[catch22] = (char *)malloc((strlen(getenv("HOME")) + 2) * sizeof(char));
				strcpy(incompletePath[catch22], getenv("HOME"));
				strcat(incompletePath[catch22], "/");
			}
			else if ((strcmp(holder, "/") == 0) && catch22 == 0) {

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

	if (fileExist(finisher) == 1 || pass == 1) {
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
	printf("%s\n", extra);
	/*	char * pathTest;
	printf("%s\n", getenv("PATH"));
	for (i = 0; i < count; i++) {
		printf("%s\n",incompletePath2[i]);
	}*/

	for (i = 0; i < count; i++) {
		if (fileExist(incompletePath2[i]) == 1) {
			return incompletePath2[i];
		}
	}
	//syntax error check if pipe command is the first thing inputed
	if (strcmp(extra, "|") == 0) {
		printf("ERROR: invalid syntax: %s\n", extra);
	}
	else {
		printf("%s\n", "file does not exist");
	}
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
	memcpy(subbuff, &name[1], strlen(name));
	subbuff[strlen(name)] = '\0';
	char * value = getenv(subbuff);
	return value;

}
void my_execute(char **cmd, int size, struct Queue* queue, int bcheck) {

	int status;
	int i;
	/*for (i = 0; i < size; i++) {
		printf("is this the number %d", i);
		printf("%s", cmd[i]);
		printf("\n");
	}*/
	//cmd = (char**)realloc(cmd, (size + 1) * sizeof(char*));
	//cmd[size] = NULL;
	pid_t pid = fork();

	if (pid == 0) {

		//Child

		execv(cmd[0], cmd);
		fprintf("Problem executing %s\n", cmd[0]);
		exit(1);
	}

	else {
		//Parent
		if (bcheck == 1) {
			RBP temprbp;
			temprbp.PIQ = queue->bcounter++;
			temprbp.PID = pid;
			temprbp.size = size - 1;
			//printf("The PID is: %d\n", pid);
			//printf("1) The command is: ");
			//for (i = 0; i < size - 1; i++) {
			//	printf("%s ", cmd[i]);
			//}
			//printf("\n");
			/*temprbp.command = malloc((size - 1) * sizeof(char *));
			for (i = 0; i < size - 1; i++) {
			  strcpy(temprbp.command[i], cmd[i]);
			}*/

			char **tempArray = malloc((size - 1) * sizeof(char *));
			for (i = 0; i < size - 1; i++) {
				(tempArray)[i] = cmd[i];
			}
			temprbp.command = tempArray;
			//printf("2) The command is: ");
			//for (i = 0; i < size - 1; i++) {
			//	printf("%s ", temprbp.command[i]);
			//}

			enqueue(queue, temprbp);
			waitpid(pid, &status, -1);
			printf("[%d]    [%d]\n", temprbp.PIQ, temprbp.PID);
		}
		else {
			waitpid(pid, &status, 0);
		}
	}
	//	cmd = (char**)realloc(cmd, (size + 1) * sizeof(char*));
}

//reallocates instruction array to hold another token
//allocates for new token within instruction array
void addToken(instruction* instr_ptr, char* tok)
{
	//extend token array to accomodate an additional token
	if (instr_ptr->numTokens == 0)
		instr_ptr->tokens = (char**)malloc(sizeof(char*));
	else
		instr_ptr->tokens = (char**)realloc(instr_ptr->tokens, (instr_ptr->numTokens + 1) * sizeof(char*));

	//allocate char array for new token in new slot
	instr_ptr->tokens[instr_ptr->numTokens] = (char *)malloc((strlen(tok) + 1) * sizeof(char));
	strcpy(instr_ptr->tokens[instr_ptr->numTokens], tok);

	instr_ptr->numTokens++;
}

void addNull(instruction* instr_ptr)
{
	//extend token array to accomodate an additional token
	if (instr_ptr->numTokens == 0)
		instr_ptr->tokens = (char**)malloc(sizeof(char*));
	else
		instr_ptr->tokens = (char**)realloc(instr_ptr->tokens, (instr_ptr->numTokens + 1) * sizeof(char*));

	instr_ptr->tokens[instr_ptr->numTokens] = (char*)NULL;
	instr_ptr->numTokens++;
}

void printTokens(instruction* instr_ptr)
{
	int i;
	printf("Tokens:\n");
	for (i = 0; i < (instr_ptr->numTokens-1); i++) {
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
