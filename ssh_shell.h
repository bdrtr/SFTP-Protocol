#ifndef ssh_shell
#define ssh_shell




struct command_create
{
    char * username;
    char * passwd;
    char * command_type;
    char * dns_name;
    int  port;
};

typedef struct command_create command;


void shell_commands(int argc, char * argv[], command * cmd);

void get_doc();

#endif // ssh_shell