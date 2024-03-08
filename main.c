#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include "ssh_prot.h"
#include "ssh_shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>


pid_t pid;


int verbosity = SSH_LOG_PROTOCOL;
int port = 22;


command * cmd;


int main(int argc, char *argv[]) {

    int rc;

    char *user_name = "guest";
    char *pass_key = "12345678";


    cmd = (command *)malloc(sizeof(command));
    shell_commands(argc, argv, cmd);

    printf("%s, %s, %s \n",cmd->command_type, cmd->passwd, cmd->username);


    ssh_session my_session = ssh_new();
    if (my_session == NULL) exit(-1);


    rc = connect_the_session(my_session, cmd->dns_name, verbosity, cmd->port);

    if (rc != SSH_OK)  {
        fprintf(stderr, "adrese bağlanırken hata meydana geldi %s\n",
                    ssh_get_error(my_session));
                    exit(-1);
    
    } else if (rc == SSH_OK) {

        printf("adrese bağlantı kuruldu \n");
    }


    if (authentication_pubkey(my_session, 
                user_name, pass_key ) == SSH_AUTH_SUCCESS) {

        printf("giris basarili \n");

        char *banner = ssh_get_issue_banner(my_session);
        printf("serverin mesaji -> %s\n", banner); 

        pid = fork();
        
        if (pid == 0 && !strcmp(cmd->command_type,"-config")) {
            printf("dosya durumu %d\n", ssh_file_control());
        
        
        } else {

            waitpid(pid, NULL, 0);
            //cocuk islemi beklemeliyim cunku excvp ile sonlancak
            //ve bende burda ana islemden devam edicem

        }

        rc = open_shell_session(my_session);
        if (rc != SSH_OK) fprintf(stderr, "hata: %s\n",ssh_get_error(my_session));

    }

    return exit_session(my_session);
}       