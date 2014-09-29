/*
 Copyright (C) 2014 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


//For some reasons we're getting segfaults at startup in std::locale on some system. The "gold" linker seems to be the culprit.
//The cause appears to be that the pthreads library isn't recognized as needed. We therefore need to force its inclusion, which
//we'll do by calling the below function.
#include <pthread.h>
extern "C" {
pthread_t selfthread = pthread_self();
}

