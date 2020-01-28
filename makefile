all: project.c
	gcc -g -Wall -o shell project.c

clean:
	$(RM) shell
