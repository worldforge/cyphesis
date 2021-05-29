/*
 Copyright (C) 2021 Erik Ogenvik

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

#ifndef CYPHESIS_PYTHONMALLOC_H
#define CYPHESIS_PYTHONMALLOC_H

/**
 * Sets up Python to use malloc rather than the default Python memory allocator.
 *
 * This makes Python slower, but it allows it to play better with things such as Valgrind
 * or AddressSanitizer. Thus it should only be called in test and debug code.
 */
void setupPythonMalloc();

#endif //CYPHESIS_PYTHONMALLOC_H
