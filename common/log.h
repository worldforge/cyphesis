#include <fstream.h>

namespace common {
  namespace log {

    extern ofstream inform_fp;
    extern ofstream debug_fp;
    extern ofstream thinking_fp;

    void debug(int level, char * msg, int op=0);
    void inform(char * msg, int op);
    void thinking(char * msg);

  }
}
