//COP4610
//Project 1 Starter Code
//example code for initial parsing

//*** if any problems are found with this code,
//*** please report them to the TA


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

int main() {
    char* token = NULL;
    char* temp = NULL;

    instruction instr;
    instr.tokens = NULL;
    instr.numTokens = 0;

    while (1) {

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

        inputAction(&instr);

        addNull(&instr);
       // printTokens(&instr);                 // <----- 	COMMENTED OFF TOKEN PRINTING
        clearInstruction(&instr);

    }

    return 0;
}


// echo function now works and error checks for all possible environmental variables
// Renamed function
void inputAction(instruction* instr_ptr){

    if(strcmp((instr_ptr->tokens)[0],"echo") == 0){


        int i;
        for (i = 1; i < instr_ptr->numTokens; i++) {
            if(((instr_ptr->tokens)[i][0]) == '$') {
							if (expandEnv((instr_ptr->tokens)[i]) == NULL) {
								printf("NO SUCH COMMAND EXISTS ");
							}
							else {
								printf("%s ",expandEnv((instr_ptr->tokens)[i]));
							}
						}
            else
                printf("%s ", (instr_ptr->tokens[i]));
        }

    }
    //else if(strcmp((instr_ptr->tokens)[0],"cd") == 0){

    //}
    else {
		printf("%s: NO SUCH COMMAND FOUND",(instr_ptr->tokens)[0]);
		}

    printf("\n");
}



// Function to return any proper environmental variable value
char* expandEnv(const char * name) {

    char subbuff[strlen(name)];
    memcpy( subbuff, &name[1], strlen(name));
    subbuff[strlen(name)] = '\0';
    char * value = getenv(subbuff);
    return value;

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
