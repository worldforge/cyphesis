// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef COMMON_LOG_H
#define COMMON_LOG_H

#include <fstream.h>

namespace common {
    namespace log {

        extern ofstream inform_fp;
        extern ofstream debug_fp;
        extern ofstream thinking_fp;

        void debugn(int level, char * msg, int op=0);
        void inform(char * msg, int op);
        void thinking(char * msg);

    }
}

#endif // COMMON_LOG_H
