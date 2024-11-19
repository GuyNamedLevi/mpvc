#include <string.h>
#include <sys/un.h>

#define SOCKBUFFERSIZE 100

typedef char json_s;

typedef struct { int fd; struct sockaddr_un socket_addr; } mpvSocket;

// Creates a socket from path to pointer
int mpvcCreateSocket(const char *filename, mpvSocket *);

// Send json message to mpv socket
int mpvcSendJson(json_s *, mpvSocket *); 

void mpvcCreateJson(json_s *, char *[]);

// toggles playback; returns  0 - playing, 1 - pausing, -1 - fail;
// accepts 0 - off, 1 - on, 2 - toggle
int mpvcPause(const int, mpvSocket *mpvsock);
// Play path
int mpvcPlay(char *path, mpvSocket *mpvsock);
// 1 == Next in playlist, 0 == Prev in playlist
int mpvcNextPrev(const int, mpvSocket *mpvsock);
// 1 == Save position and exit, 0 == just exit
int mpvcStop(const int, mpvSocket *mpvsock);
