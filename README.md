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
  2) Background Processes - some processes such as 'ls' processes<br/>
  run perfectly as a background process. Others, like some redirects<br/>
  or pipes cause the program to crash. This is most likely due to a path<br/>
  resolution error when the & is removed from the command; however, we do<br/>
  not understand why this does not affect ls commands.
  3) Printing completed background commands - when trying to save the<br/>
  commands to print when the processes are finished there is a loss of<br/>
  memory and nothing will print from the array. This is most likely a<br/>
  pointer issue we did not have enough time to fix.
  
