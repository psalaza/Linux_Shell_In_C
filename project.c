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
void mypipe(int fd[], char *c1[], char* c2[], char *c3[], int t);
void addToken(instruction* instr_ptr, char* tok);
void printTokens(instruction* instr_ptr);
void clearInstruction(instruction* instr_ptr);
void addNull(instruction* instr_ptr);
char* expandEnv(const char * name);
void inputAction(instruction* instr_ptr, struct Queue* queue, int *cc);
char* path(const char * name, int pass);
char * checkForPath(char *extra);
int fileExist(char * absolutePath);
void execute(char **commands, int size, struct Queue* queue, int bcheck, int *cc);

int main() {
	char* token = NULL;
	char* temp = NULL;

	instruction instr;
	instr.tokens = NULL;
	instr.numTokens = 0;

	struct Queue* cmdqueue = createQueue(100);

	int commandcount = 0;

	while (1) {

		if (isEmpty(cmdqueue) == 0) {
			int tempint, status, i;
			for (tempint = 0; tempint < cmdqueue->bcounter; tempint++) {
				if (waitpid(cmdqueue->array[tempint].PID, &status, WNOHANG) != 0) {}
				else {
					printf("[%d]+    [", cmdqueue->array[tempint].PIQ);
					for (i = 0; i < cmdqueue->array[tempint].size - 1; i++) {
						printf("%s ", cmdqueue->array[tempint].command[i]);
					}
					printf("\n");
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
		addNull(&instr);                //add null early for use in execute
		inputAction(&instr, cmdqueue, &commandcount);
		printTokens(&instr);
		clearInstruction(&instr);

		// Testing to see if queue works
		// printf("%d dequeued from queue\n\n", dequeue(cmdqueue));
	}

	return 0;
}


// echo function now works and error checks for all possible environmental variables
// Renamed function
void inputAction(instruction* instr_ptr, struct Queue* queue, int *cc) {
	int i, syncheck;

	char *check;
	char *recieve;
	char *check2;
	char *check4output;
	char *check4input;
	char *check4other;
	char * checkforNull;
	char dir[100];
	getcwd(dir, 100);


	int pid, status;
	int fd[2];

	char *CMD1[4];
	char *CMD2[4];
	char *CMD3[4];
	int c1 = 0, c2 = 0, c3 = 0;
	int s1 = sizeof(CMD1) / sizeof(CMD1[0]);
	int p;
	for (p = 0; p < s1; p++) {
		CMD1[p] = NULL;
		CMD2[p] = NULL;
		CMD3[p] = NULL;
	}
	int temp = 0;
	int k;
	int pp = 0;
	int ret;
	for (k = 0; k < instr_ptr->numTokens; k++) {
		if ((instr_ptr->tokens)[k] != NULL) {
			ret = strcmp((instr_ptr->tokens)[k], "|");
			if (ret == 0)
				break;
		}

	}

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
						if (strcmp((instr_ptr->tokens)[1], "..") == 0) {                   //path needs a slash to help it understand if one of these values appear

							instr_ptr->tokens[1] = (char *)realloc((instr_ptr->tokens)[1], (strlen((instr_ptr->tokens)[1]) + 1) * sizeof(char));
							strcat(instr_ptr->tokens[1], "/");

						}
						else if (strcmp((instr_ptr->tokens)[1], ".") == 0) {
							instr_ptr->tokens[1] = (char *)realloc((instr_ptr->tokens)[1], (strlen((instr_ptr->tokens)[1]) + 1) * sizeof(char));
							strcat(instr_ptr->tokens[1], "/");

						}
						else if (strcmp((instr_ptr->tokens)[1], "~") == 0) {
							instr_ptr->tokens[1] = (char *)realloc((instr_ptr->tokens)[1], (strlen((instr_ptr->tokens)[1]) + 1) * sizeof(char));
							strcat(instr_ptr->tokens[1], "/");

						}
						checkforNull = path((instr_ptr->tokens)[1], 0);
						if (checkforNull != NULL) {

							setenv("PWD", checkforNull, 1);
						}

					}
				}
			}
			else
				printf("Too many arguments\n");
		}
		else if (strcmp((instr_ptr->tokens)[0], "jobs") == 0) {
			int i;
			if (isEmpty(queue)) {
				printf("All background processes complete\n");
			}
			else {
				for (i = 0; i < queue->size; i++) {
					printf("[%d]+ [%d] [%s]\n", queue->array[i].PIQ, queue->array[i].PID, queue->array[i].command[0]);
					// need to create command print function
				}
			}
		}
		else if (strcmp((instr_ptr->tokens)[0], "exit") == 0) {
			printf("Exiting now!\n\t Commands executed: %d\n", *cc);
			if (isEmpty(queue)) {
				exit(1);
			}
			else {
				while (1) {
					int tempint, status, i;
					for (tempint = 0; tempint < queue->bcounter; tempint++) {
						if (waitpid(queue->array[tempint].PID, &status, WNOHANG) != 0) {}
						else {
							printf("[%d]+    [", queue->array[tempint].PIQ);
							//queue->array[tempint].command[0]);
							dequeue(queue);
						}
					}
				}
			}
		}
		//pipe command found in user input
		else if (ret == 0) {

			for (k = 0; k < instr_ptr->numTokens; k++) {

				if ((instr_ptr->tokens)[k] != NULL) {
					int q = 0;
					if (strcmp((instr_ptr->tokens)[k], "|") == 0) {
						q = 1;
						temp = temp + 1;
					}
					if (q == 0) {

						switch (temp) {

						case 0:
							CMD1[c1] = (instr_ptr->tokens)[k];
							c1++;
							break;
						case 1:
							CMD2[c2] = (instr_ptr->tokens)[k];
							c2++;
							break;
						case 2:
							CMD3[c3] = (instr_ptr->tokens)[k];
							c3++;
							break;
						}
					}
				}

			}


			if ((instr_ptr->tokens)[2] == NULL) {
				printf("ERROR: invalid syntax, no 2nd argument found\n");
			}
			else {

				// Adds to command counter
				int tempint = *cc;
				*cc = tempint + 1;

				pipe(fd);
				switch (pid = fork()) {
				case 0:
					//child
					mypipe(fd, CMD1, CMD2, CMD3, s1);
					exit(0);

				default:
					//parent
					while ((pid = wait(&status)) != -1)
						fprintf(stderr, "process %d exits with %d\n", pid, WIFSTOPPED(status));
					break;

				}

			}
		}
		else {
			for (i = 0; i < instr_ptr->numTokens; i++) {
				if ((instr_ptr->tokens)[i] != NULL) {

					check = strrchr(instr_ptr->tokens[i], '/');            //checks if it is path
					check4output = strrchr(instr_ptr->tokens[i], '>');     //checks if their is an io redirection to put infrormation
					check4input = strrchr(instr_ptr->tokens[i], '<');
					if (check4output != NULL) {
						recieve = NULL;

						if (i + 5 == instr_ptr->numTokens) {

							check4other = strrchr(instr_ptr->tokens[i + 2], '<');
							if (check4other != NULL) {

								if (i == 0) {
									printf("< canot be at the start of the input");
								}
								else {
									FILE *target = fopen(path(instr_ptr->tokens[i + 3], 0), "r");    //read file
									FILE * gate = fopen(path(instr_ptr->tokens[i + 1], 1), "w");     //write/create file
									int fd = fileno(target);                                         //get file number
									int fc = fileno(gate);                                           //get file number
									instr_ptr->tokens[i] = NULL;                                     //so it only execute to this point
									if (target != NULL) {
										int status;
										pid_t pid = fork();

										switch (pid) {
										case 0:
											close(STDIN_FILENO);
											dup(fd);                                                 //puts file at standared input 
											close(STDOUT_FILENO);
											dup(fc);											     //put file at standared input
											execute(instr_ptr->tokens, i, queue, syncheck, cc);
											close(fd);
											close(fc);
											exit(1);
											break;
										default:
											waitpid(pid, &status, 0);                                //wait so that it doesnt start so it doesnt start something else till this part is done
											close(fc);
											close(fd);
											break;

										}
									}
								}
							}
							else {
								printf("there seems to ba file after after input");

							}
						}
						else if (i == 0) {
							printf("the <  sign cannot be at the start of the input");
						}
						else if (i + 2 == instr_ptr->numTokens) {
							printf("< then sign cannot be at the end of the input");
						}
						else if (i + 3 == instr_ptr->numTokens) {
							FILE * gate = fopen(path(instr_ptr->tokens[i + 1], 1), "w");              //write/create file
							int fd = fileno(gate);                                                    //gets fileno
							instr_ptr->tokens[i] = NULL;                                              //information for end of file
							switch (fork()) {                                                         //this doenst not have a major effect on program so i just let it run
							case 0:
								close(STDOUT_FILENO);
								dup(fd);
								execute(instr_ptr->tokens, i, queue, syncheck, cc);
								close(fd);
								exit(1);
								break;
							default:
								close(fd);
								break;

							}
						}

						else {
							printf("there seems to ba file after after output");
						}

						break;
					}
					else if (check4input != NULL) {                                                  //sees if thier is an <
						recieve = NULL;

						if (i + 5 == instr_ptr->numTokens) {
							check4other = strrchr(instr_ptr->tokens[i + 2], '>');                   //checks for >
							if (check4other != NULL) {
								if (i == 0) {
									printf("the > sign canot be at the start of the input");
								}
								else {
									FILE *target = fopen(path(instr_ptr->tokens[i + 1], 0), "r");   // same as above funtion with checkother accept read is what this program 
									FILE * gate = fopen(path(instr_ptr->tokens[i + 3], 1), "w");	//sees first and write is what it see second
									int fd = fileno(target);
									int fc = fileno(gate);
									int status;

									instr_ptr->tokens[i] = NULL;
									if (target != NULL) {
										pid_t pid = fork();
										switch (pid) {
										case 0:
											close(STDIN_FILENO);
											dup(fd);
											close(STDOUT_FILENO);
											dup(fc);

											execute(instr_ptr->tokens, i, queue, syncheck, cc);
											close(fd);
											close(fc);
											exit(1);
											break;
										default:
											close(fc);
											close(fd);
											waitpid(pid, &status, 0);
											break;

										}
									}

								}
							}
							else {
								printf("there seems to ba file after after input");

							}
						}
						else if (i == 0) {
							printf("the > canot be at the start of the input");
						}
						else if (i + 2 == instr_ptr->numTokens) {
							printf("the >  sign canot be at the end of the input");
						}
						else if (i + 3 == instr_ptr->numTokens) {
							FILE * gate = fopen(path(instr_ptr->tokens[i + 1], 0), "r");    //just reads
							if (gate != NULL) {
								int fd = fileno(gate);
								instr_ptr->tokens[i] = NULL;
								int status;
								pid_t pid = fork();

								switch (pid) {
								case 0:
									close(STDIN_FILENO);                                    //close standared input so it can put a new file in standaredinput
									dup(fd);
									execute(instr_ptr->tokens, i, queue, syncheck, cc);
									close(fd);
									exit(1);                                                //exit so that it doesnt end up in an endless loop
									break;
								default:
									waitpid(pid, &status, 0);
									close(fd);
									break;
								}
							}
							else {
							//	printf("this file does not exist");

							}
						}
						else {
							// printf("there seems to ba file after after input");
						}
						break;
					}
					else if (check == NULL && i == 0) {           //checks if it is a external command
						//if (check2Complete != 0) {
						recieve = checkForPath(instr_ptr->tokens[i]);
						//printf("%s%d", recieve, i);
					//}
				//	else {
					//	recieve = instr_ptr->tokens[i];
					//}
					}
					else {
						recieve = instr_ptr->tokens[i];
						//recieve = path(instr_ptr->tokens[i], 0);
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
				execute(instr_ptr->tokens, instr_ptr->numTokens, queue, syncheck, cc);
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
	int catch22 = 0;                                //checks to see if their is already something in incmplete path
	int begining = 0;                               //keeps track of where to start in the array
	char *ptr;
	char *file;                                     //to check if their is a file in the wrong place
	char *finisher;                                 //finsished path
	char **incompletePath;                          //array of strings that the path is seaperated into
	char *holder = (char*)malloc((strlen(name) + 1) * sizeof(char));

	for (i = 0; i < strlen(name) + 1; i++) {        //goes through cstring and breaks it apart at ceartin values
													//it then checks those values to see if their is an error
													//like a file in  the wrong place or if their is something that needs to be repplaced

													//pull out special characters and make them into a separate token in the instruction
		if (name[i] == '/' || name[i] == '\0') {

			i++;
			memcpy(holder, name + begining, i - begining);
			holder[i - begining] = '\0';

			if (catch22 == 0) {                      //increases the size of incomplete path array

				incompletePath = (char**)malloc(sizeof(char*));
			}
			else {

				incompletePath = (char**)realloc(incompletePath, (catch22 + 1) * sizeof(char*));
			}

			file = strrchr(holder, '.');
			if (strcmp(holder, "./") == 0) {		 //if thiers is ./ then it replaces it with current working direcory
													 //after allocating space for it in incomplete paths. and adds and ending slash
				if (name[i] != '\0') {
					incompletePath[catch22] = (char *)malloc((strlen(getenv("PWD")) + 2) * sizeof(char));
					strcpy(incompletePath[catch22], getenv("PWD"));
					strcat(incompletePath[catch22], "/");
				}
				else {
					incompletePath[catch22] = (char *)malloc((strlen(getenv("PWD")) + 1) * sizeof(char));
					strcpy(incompletePath[catch22], getenv("PWD"));

				}
			}
			else if (strcmp(holder, "../") == 0) {   //if thiers is ../ then it replaces it with directory before current working direcory
													 //after allocating space for it in incomplete paths. and adds and ending slash


				char* pWD = getenv("PWD");
				ptr = strrchr(pWD, '/');             //finds the last slash and replaces it with a null charecter

				*ptr = '\0';
				if (name[i] != '\0') {
					incompletePath[catch22] = (char *)malloc((strlen(pWD) + 2) * sizeof(char));
					strcpy(incompletePath[catch22], pWD);
					strcat(incompletePath[catch22], "/");
				}
				else {
					incompletePath[catch22] = (char *)malloc((strlen(pWD) + 1) * sizeof(char));
					strcpy(incompletePath[catch22], pWD);
				}

				*ptr = '/';							//puts charecter back
			}
			else if (file != NULL && name[i - 1] != '\0') {
				//checks if it is a file
				printf("%s\n", "there is a file where it is not suppose to be there");
				return NULL;
			}
			else if ((strcmp(holder, "~/") == 0) && catch22 == 0) {
				//if thiers is ~/ then it replaces it with home direcory
				//after allocating space for it in incomplete paths. and adds and ending slash


				if (name[i] != '\0') {
					incompletePath[catch22] = (char *)malloc((strlen(getenv("HOME")) + 2) * sizeof(char));
					strcpy(incompletePath[catch22], getenv("HOME"));
					strcat(incompletePath[catch22], "/");
				}
				else {
					incompletePath[catch22] = (char *)malloc((strlen(getenv("HOME")) + 1) * sizeof(char));
					strcpy(incompletePath[catch22], getenv("HOME"));

				}
			}
			else if ((strcmp(holder, "/") == 0) && catch22 == 0) {
				//if thiers is / then it replaces it with root direcory
				//after allocating space for it in incomplete paths. and adds and ending slash

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

				if (catch22 == 0) {                    //default replaces it to working directory
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

	for (i = 0; i < catch22; i++) {                    //puts string to together and puts it in a cstring from incomplete pat
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

	if (fileExist(finisher) == 1 || pass == 1) {              //checks for file existence
		return finisher;
	}

	printf("this file does not exist");
	return NULL;


}
char * checkForPath(char *extra) {
	int i;
	int begining = 0;                    //where to begin the next parst
	int count = 0;                       //size of incompletePath2
	char **incompletePath2;              //holds possible absolute paths
	char * paths = getenv("PATH");       //gets possible paths
	char *holder2 = (char*)malloc((strlen(paths) + 1) * sizeof(char));
	for (i = 0; i < strlen(paths) + 1; i++) {//parses path by :
		if (paths[i] == ':' || paths[i] == '\0') {

			if (count == 0) {                //allocates size
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
	//printf("%s\n", extra);
	/*	char * pathTest;
	printf("%s\n", getenv("PATH"));
	for (i = 0; i < count; i++) {
		printf("%s\n",incompletePath2[i]);
	}*/

	for (i = 0; i < count; i++) {          //returns after confirming file exsitense
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
int fileExist(char * absolutePath) {      //checks if you can access file
	if (access(absolutePath, F_OK) == -1) //returns 1 if you can access file
	{
		return 0;
	}
	return 1;
}


void mypipe(int fd[], char *c1[], char* c2[], char *c3[], int t) {

	int s1 = t;
	int i;
	printf("PIPING!\n");
	int pid;

	char temp[50];
	strcpy(temp, "/bin/");
	strcat(temp, c1[0]);

	char temp2[50];
	strcpy(temp2, "/bin/");
	strcat(temp2, c2[0]);
	if (c3[0] != NULL) {
		char temp3[50];
		strcpy(temp3, "/bin/");
		strcat(temp3, c3[0]);
		c3[0] = temp3;
	}
	c1[0] = temp;
	c2[0] = temp2;

	/*
	*/


	char *d1[] = { c1[0],c1[1],c1[2],c1[3] };
	char *d2[] = { c2[0],c2[1],c2[2],c2[3] };
	char *d3[] = { c3[0],c3[1],c3[2],c3[3] };
	printf("Printing command arrays just to show\nPIPE RESULTS DO NOT SHOW UNTIL AFTER USER ENTERS EXIT\nwe also need to add command counter when pipe is used\n");
	for (i = 0; i < s1; i++) {
		printf("CMD1: %s\n", d1[i]);
	}
	for (i = 0; i < s1; i++) {
		printf("CMD2: %s\n", d2[i]);
	}
	for (i = 0; i < s1; i++) {
		printf("CMD3: %s\n", d3[i]);
	}

	switch (pid = fork()) {
	case 0:
		//child
		dup2(fd[0], 0);
		close(fd[1]);
		execv(d2[0], d2);
		perror(d2[0]);
	default:
		//parent
		dup2(fd[1], 1);
		close(fd[0]);
		execv(d1[0], d1);
		perror(d1[0]);

	}
}

// Function to return any proper environmental variable value
char* expandEnv(const char * name) {

	char subbuff[strlen(name)];
	memcpy(subbuff, &name[1], strlen(name));
	subbuff[strlen(name)] = '\0';
	char * value = getenv(subbuff);
	return value;

}
void execute(char **commands, int size, struct Queue* queue, int bcheck, int *cc) {

	int status;
	int i;
	/*for (i = 0; i < size; i++) {
		printf("is this the number %d", i);
		printf("%s", commands[i]);
		printf("\n");
	}*/
	//commands = (char**)realloc(commands, (size + 1) * sizeof(char*));
	//commands[size] = NULL;
	pid_t pid = fork();                                          //based on slides gets pid for use i wait

	if (pid == 0) {                                              //if it is a child do this otherwise wait



		execv(commands[0], commands);
		fprintf("Problem executing %s\n", commands[0]);
		exit(1);
	}

	else {

		if (bcheck == 1) {
			RBP temprbp;
			temprbp.PIQ = queue->bcounter++;
			temprbp.PID = pid;
			temprbp.size = size - 1;
			//printf("The PID is: %d\n", pid);
			//printf("1) The command is: ");
			//for (i = 0; i < size - 1; i++) {
			//	printf("%s ", commands[i]);
			//}
			//printf("\n");
			/*temprbp.command = malloc((size - 1) * sizeof(char *));
			for (i = 0; i < size - 1; i++) {
			  strcpy(temprbp.command[i], commands[i]);
			}*/

			char **tempArray = malloc((size - 1) * sizeof(char *));
			for (i = 0; i < size - 1; i++) {
				(tempArray)[i] = commands[i];
			}
			temprbp.command = tempArray;
			/*printf("2) The command is: ");
			for (i = 0; i < size - 1; i++) {
				printf("%s ", temprbp.command[i]);
			}*/

			enqueue(queue, temprbp);
			waitpid(pid, &status, -1);
			printf("[%d]    [%d]\n", temprbp.PIQ, temprbp.PID);
		}
		else {
			waitpid(pid, &status, 0);
		}
	}
	//	commands = (char**)realloc(commands, (size + 1) * sizeof(char*));
	int tempint = *cc;
	*cc = tempint + 1;
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
	for (i = 0; i < (instr_ptr->numTokens - 1); i++) {
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
