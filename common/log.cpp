extern "C" {
    #include <stdio.h>
}

#include "log.h"
#include "const.h"

namespace common {
namespace log {

ofstream inform_fp;
ofstream debug_fp;
ofstream thinking_fp;

void debug(int level, char * msg, int op=0)
{
    if (consts::debug_level>=level) {
        if (debug_fp) {
            debug_fp << msg << endl;
            if (op!=0) {
                debug_fp << op << endl;
            }
            debug_fp << flush;
        } else {
            cout << msg;
            if (op!=0) {
                cout << op;
            }
        }
    }
}

void inform(char * msg, int op)
{
    if (consts::debug_level) {
        if (inform_fp) {
            inform_fp << msg << endl << op << endl << flush;
        }
        else {
            cout << msg;
        }
    }
}

void thinking(char * msg)
{
    if (consts::debug_thinking) {
        if (thinking_fp) {
            thinking_fp << msg << endl << flush;
        } else {
            cout << msg;
        }
    }
}

}
}
