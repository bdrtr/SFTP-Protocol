#ifndef ssh_prot
#define ssh_prot

#include <libssh/libssh.h>

int connect_the_session(ssh_session session, char * dns, int verbosity, int port);

int exit_session(ssh_session session);

int authenticate_none(ssh_session session);

int kbhit();

int authentication_wpasswd(ssh_session session, char * username, char * passwd);

int authentication_pubkey(ssh_session session, char *username, char *passkey);

int request_interactive_shell(ssh_channel channel);

int open_shell_session(ssh_session session);

int ssh_file_control();

#endif // ssh_prot