typedef int bad_type; // Remove this to get unset type reporting

#include <fstream.h>

namespace log {

extern ofstream inform_fp;
extern ofstream debug_fp;
extern ofstream thinking_fp;

void debug(int level, char * msg, bad_type op=0);
void inform(char * msg,bad_type op);
void thinking(char * msg);

}
