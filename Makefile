dir = libs_ssh
LIBS = -lssh

program: main.c $(dir)/ssh_prot.c $(dir)/ssh_shell.c
	gcc -o program main.c $(dir)/ssh_prot.c $(dir)/ssh_shell.c $(LIBS)