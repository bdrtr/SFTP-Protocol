#include "ssh_prot.h"
#include <stdlib.h>
#include <stdio.h>


int connect_the_session(ssh_session session, char * dns, int verbosity, int port) {

    printf("dns: %s\n", dns);
    ssh_options_set(session, SSH_OPTIONS_HOST, dns);
    ssh_options_set(session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
    ssh_options_set(session, SSH_OPTIONS_PORT, &port);

    return ssh_connect(session);    
}

int exit_session(ssh_session session) {

    printf("çıkılıyor .. \n");
    ssh_disconnect(session);
    ssh_free(session);
}


int authentication_wpasswd(ssh_session session, char * username, char * passwd) {

    int rc;
    rc = ssh_userauth_password(session, username, passwd);
    if (rc != SSH_AUTH_SUCCESS) return SSH_AUTH_DENIED;
    return SSH_AUTH_SUCCESS;
}

int authenticate_none(ssh_session session)
{
  //ancak sunucu kullanıcı adı ve şifre boş bırakılarak girmeye
  //izin veriyorsa bunu kullanabilirsiniz

  int rc;
 
  rc = ssh_userauth_none(session, NULL);
  return rc;
}

int authentication_pubkey(ssh_session session, char *username, char *passkey) {

    int rc;

    //bu giriş için ssh_config (istemci) dosyasında tanımlı bir
    //gerekli ayarlamalar yapılmalıdır
    //~/ssh/  dizinin altında bir ssh key generator ile rsa dosyası 
    //oluşturduğunuzu ve sshd_config (sunucu) dosyasında ilgili
    //bölümün aktif olduğunu, istemci dosyası için bu anahtarın 
    //izinlerinin aktif olduğunu varsayarım. 


    //önemli -> passkey ve passwd şifreniz aynı olarak varsayılır burda eğer 
    //passkey doğru değilse onu birde normal şifreli giriş için dener

    rc = ssh_userauth_publickey_auto(session, username, passkey);
    if (rc == SSH_AUTH_ERROR) {
        fprintf(stderr, "kimlik dogrulama basarisiz. %s\n",ssh_get_error(session));
        return SSH_AUTH_ERROR;
    
    } else if(rc == SSH_AUTH_DENIED) {

        //eğer publickey basarısız olursa normal giris denenir
        rc = authentication_wpasswd(session, username, passkey);

        if (rc != SSH_AUTH_SUCCESS) {

            fprintf(stderr, "kimlik eslestirme  basarisiz. %s\n",ssh_get_error(session));
            return SSH_AUTH_DENIED;    
        }    
        return SSH_AUTH_SUCCESS;
    
    } else if (rc == SSH_AUTH_SUCCESS) {

        fprintf(stderr, "kimlik eslestirme  basarili. %s\n",ssh_get_error(session));
        return SSH_AUTH_SUCCESS;
    } 

    return rc;
}

  
int kbhit () {

    struct timeval tv = {0L, 0L};
    fd_set fds;

    FD_ZERO(&fds);
    FD_SET(0, &fds);

    return select(1, &fds, NULL, NULL, &tv);
}



int request_interactive_shell(ssh_channel channel) {

    int rc;
    char buffer[256];
    int nbytes, nwritten;

    rc = ssh_channel_request_pty(channel);
    if (rc != SSH_OK) return rc;

    rc = ssh_channel_change_pty_size(channel, 80,24);
    if (rc != SSH_OK) return rc;

    rc = ssh_channel_request_shell(channel);
    if (rc != SSH_OK) return rc;

    while (ssh_channel_is_open(channel) && !ssh_channel_is_eof(channel))  {
        nbytes = ssh_channel_read_nonblocking(channel, buffer, sizeof(buffer), 0);
        if (nbytes < 0) return SSH_ERROR;
        if (nbytes > 0) {
            nwritten = write(1, buffer, nbytes);
            if (nwritten != nbytes) return SSH_ERROR;
    
            if (!kbhit()) {

                usleep(50000L); // 0.05 second
                continue;
            }
    
            nbytes = read(0, buffer, sizeof(buffer));
            if (nbytes < 0) return SSH_ERROR;
            if (nbytes > 0)
            {
            nwritten = ssh_channel_write(channel, buffer, nbytes);
            if (nwritten != nbytes) return SSH_ERROR;
            }
        }

    }    

    ssh_channel_close(channel);
    ssh_channel_send_eof(channel);
    ssh_channel_free(channel);

    return SSH_OK;
}


int open_shell_session(ssh_session session) {

    ssh_channel channel;
    char buffer[256];
    int rc, nbytes;

    channel = ssh_channel_new(session);

    if (channel == NULL) return SSH_ERROR;

    rc = ssh_channel_open_session(channel);

    if (rc != SSH_OK) {

        ssh_channel_free(channel);
        return rc;
    }


    rc = ssh_channel_request_exec(channel, "ls -l");
    if (rc != SSH_OK)   {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
    }

    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);

    while (nbytes > 0 ) {
        if (fwrite(buffer, 1, nbytes, stdout) != nbytes) {
            ssh_channel_close(channel);
            ssh_channel_free(channel);
        }

        nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    }

    

    //rc = request_interactive_shell(channel);//interaktif terminal isteği
    //if (rc != SSH_OK) return SSH_ERROR;
    //printf("rc value %d\n", rc);

    ssh_channel_close(channel);
    ssh_channel_send_eof(channel);
    ssh_channel_free(channel);

    return SSH_OK;

}


int ssh_file_control() {

    int rc;

    char * file_path = "/etc/ssh/ssh_config";
    char *command[] = {"sudo", "nano", file_path, NULL};

    if (access(file_path, F_OK) != -1 ) {
        
        execvp(command[0], command);
        perror("execvp failed");

        return 1;
    }

     

    return -1;
}
