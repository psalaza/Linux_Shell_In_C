# OpSys1
Project 1: Implementing a Shell

Group Members: Charles Harris, Pedro Salazar, Spencer Lewis

File Listing:<br/>
gitlog.txt, README.md, makefile,  project.c

Makefile Description:<br/>
In order to run the implemented shell:<br/>
  1) To compile project.c run:
  > make
  2) To begin the program run:
  > shell
  3) When finished run:
  > make clean

Division of Labor:<br/>

  Charles Harris: Shortcut and $PATH Resolution, Execution,<br/>
  I/O Redirection, Comments and Documentation<br/>
  
  Pedro Salazar: Pipes, cd command, echo ARGS command,<br/>
  Comments and Documentation<br/>
  
  Spencer Lewis: Set up github (parsing file), Env Variables,<br/>
  Prompt, Background Processing, exit & jobs commands<br/>
  Comments and Documentation<br/>
  
Known Bugs:<br/>
  1) Extra Spaces - Adding extra spaces to the end of a command<br/>
  can cause the parser to misread the command<br/>
  2) 
