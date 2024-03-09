#include "ssh_shell.h"
#include "stdio.h"
#include "stdlib.h"
#include <string.h>


void shell_commands(int argc, char *argv[], command * cmd) {

    //standarts
    cmd->dns_name = "compiler.";
    cmd->port = 22;

    //program -> 0 guest->1  12345678->2 -config->3
    cmd->username = argv[1];
    cmd->passwd = argv[2];
    cmd->command_type = argv[3];

    
    if (!strcmp(argv[3], "-hostname")) cmd->dns_name=argv[4];
    if (!strcmp(argv[3], "-p")) cmd->port=atoi(argv[4]);

    if (!strcmp(argv[3], "-doc")) get_doc();
}

int set_user_passwd(char * username, char * passwd) {


}


void get_doc() {

    FILE * fd;
    char satir[100];

    fd = fopen("doc.txt","r");
    
    if (fd == NULL) {
        printf("dosya acilmadi \n");
        return;
    }

    while(fgets(satir, sizeof(satir), fd) != NULL) {
        printf("%s", satir);
    }

    fclose(fd);
    return;
}
