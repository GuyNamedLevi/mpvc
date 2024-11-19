#define cmdTwoCharsToInt(c1,c2) (c1 << 8) | (unsigned char)c2
#define cmdCount(i) (char[]){(char)i}
#define MPVCGETPAUSE (char *[]){ cmdCount(2), "get_property", "pause" }
#define MPVCSETPAUSE(m) (char *[]){ cmdCount(3), "set_property", "pause", m ? "yes" : "no" }
#define MPVCLOADFILE(path) (char *[]){ cmdCount(2), "loadfile", path }
#define MPVCNEXT (char *[]){ cmdCount(1), "playlist-next" } 
#define MPVCPREV (char *[]){ cmdCount(1), "playlist-prev" } 
#define MPVCQUIT (char *[]){ cmdCount(1), "quit" }
#define MPVCQUITSAVE (char *[]){ cmdCount(1), "quit_watch_later" }

typedef enum {
  CMD_PAUSE = cmdTwoCharsToInt('p','a'),
  CMD_PLAY = cmdTwoCharsToInt('p','l'),
  CMD_NEXT = cmdTwoCharsToInt('n','e'),
  CMD_PREV = cmdTwoCharsToInt('p','r'),
  CMD_STOP = cmdTwoCharsToInt('s','t')
} Command;
