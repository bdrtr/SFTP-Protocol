program: main.c ssh_prot.c ssh_shell.c
	gcc -o program main.c ssh_prot.c ssh_shell.c -lssh 
