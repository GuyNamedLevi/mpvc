#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "mpvc.h" 
#include "commands.h"

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)


int mpvcCreateSocket(const char *filename, mpvSocket *mpvsock) 
{
    // Create socket and add filename
    mpvsock->fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (mpvsock->fd < 0)
        handle_error("socket");
    mpvsock->socket_addr.sun_family = AF_UNIX;
    strcpy(mpvsock->socket_addr.sun_path, filename);
    if (connect(mpvsock->fd, (struct sockaddr *)&mpvsock->socket_addr, sizeof(mpvsock->socket_addr.sun_path)) < 0)
        handle_error("connect");
    
    return 0;
} 

int mpvcSendJson(json_s *json, mpvSocket *mpvsock) {
    // Send json to mpvsock
    if (send(mpvsock->fd, json, strlen(json), 0) < 0) 
        handle_error("sending");
    
    // Recieve awnser
    int recievedBytes;
    if ((recievedBytes = recv(mpvsock->fd, json, SOCKBUFFERSIZE, 0)) < 0 || recievedBytes > SOCKBUFFERSIZE - 1) 
        handle_error("recieve");
    
    json[recievedBytes] = '\0';

    return 0;
}

void mpvcCreateJson(json_s *json, char *commands[]) {
    // Using tmp pointer for persistent pointer arithmetic.
    json_s *tmp = json;

    // Formating the commands into json.
    for (char *v = "{\"command\":["; *v !='\0';)
        *tmp++ = *v++; 
    for (int i = 0; i < **commands; i++) { // Number of commands/args is the first item.
        *tmp++ = '\"';
        for (char *v = *(commands + i + 1); *v != '\0';)
            *tmp++ = *v++;
        *tmp++ = '\"'; *tmp++ = ',';
    }
    // Looks ugly, but is enough here.
    *--tmp = ']'; *++tmp = '}';
    *++tmp = '\n'; *++tmp = '\0';
}

int mpvcPause(int mode, mpvSocket *mpvsock) {
    json_s *json = malloc(SOCKBUFFERSIZE);
    if (json == "") {
        errno = ENOMEM;
        handle_error("malloc");
    }
    
    switch(mode) {
    case 2:
        //function to toggle
        mpvcCreateJson(json, MPVCGETPAUSE);
        mpvcSendJson(json, mpvsock);
        // Non flexible parsing, but for now it works.
        mode = *(json + 8) == 'f' ? 1 : 0;
    case 1:
    case 0:
        mpvcCreateJson(json, MPVCSETPAUSE(mode));
        mpvcSendJson(json, mpvsock);
        break;
    default:
        errno = EINVAL; 
        handle_error("arguments");
    };
    
    free(json);
    return 0;
} 

int mpvcPlay(char *path, mpvSocket *mpvsock) {
    json_s *json = malloc(SOCKBUFFERSIZE < sizeof(path) ? SOCKBUFFERSIZE : sizeof(path) + SOCKBUFFERSIZE);
    if (json == "") { 
        errno = ENOMEM;
        handle_error("malloc");
    }

    mpvcCreateJson(json, MPVCLOADFILE(path));
    mpvcSendJson(json, mpvsock);

    free(json);
    return 0;
}

int mpvcNextPrev(const int m, mpvSocket *mpvsock) {
    json_s *json = malloc(SOCKBUFFERSIZE);
    if (json == "") { 
        errno = ENOMEM;
        handle_error("malloc");
    }
    
    mpvcCreateJson(json, m ? MPVCNEXT : MPVCPREV);
    mpvcSendJson(json, mpvsock);

    free(json);
    return 0;
}

int mpvcStop(const int m, mpvSocket *mpvsock) {
    json_s *json = malloc(SOCKBUFFERSIZE);
    if (json == "") { 
        errno = ENOMEM;
        handle_error("malloc");
    }
    
    mpvcCreateJson(json, m ? MPVCQUITSAVE : MPVCQUIT);
    mpvcSendJson(json, mpvsock);

    free(json);
    return 0;
}

int main(int argc, char *argv[]) {
    char *envsocket = getenv("MPVSOCKET");
    const char *filename = envsocket == "" ? envsocket : "/tmp/mpvsocket";
    mpvSocket mpvsock;
    mpvcCreateSocket(filename, &mpvsock);

    Command cmd = cmdTwoCharsToInt(argv[1][0],argv[1][1]);
    switch (cmd) {
        case CMD_PAUSE:
            if (argc > 3) { 
                errno = EINVAL;
                handle_error("arguments"); 
            }
            mpvcPause(argc == 3 ? argv[2][0]-'0' : 2, &mpvsock);
            break;
        case CMD_PLAY:
            if (argc != 3) { 
                errno = EINVAL;
                handle_error("arguments"); 
            }
            mpvcPlay(argv[2], &mpvsock);
            break;
        case CMD_NEXT:
            if (argc != 2) { 
                errno = EINVAL;
                handle_error("arguments"); 
            }
            mpvcNextPrev(1, &mpvsock);
            break;
        case CMD_PREV:
            if (argc != 2) { 
                errno = EINVAL;
                handle_error("arguments"); 
            }
            mpvcNextPrev(0, &mpvsock);
            break;
        case CMD_STOP: 
            if (argc > 3) { 
                errno = EINVAL;
                handle_error("arguments"); 
            }
            mpvcStop(argc == 3 ? argv[2][0]-'0' : 0, &mpvsock);
            break;            
    }
    
    close(mpvsock.fd);

    return 0;
}
