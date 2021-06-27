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
#include "PythonMalloc.h"
#include <Python.h>
#include <iostream>

static void* Python_Malloc(void* ctx, size_t size)
{
    return malloc(size ? size : 1);
}

static void* Python_Realloc(void* ctx, void* ptr, size_t size)
{
    return realloc(ptr, size ? size : 1);
}

static void* Python_Calloc(void* ctx, size_t nelem, size_t elsize)
{
    if (nelem == 0 || elsize == 0) {
        nelem = 1;
        elsize = 1;
    }
    return calloc(nelem, elsize);
}

static void Python_Free(void*, void* ptr)
{
    free(ptr);
}


void setupPythonMalloc()
{
    PyMemAllocatorEx alloc = {NULL, Python_Malloc, Python_Calloc, Python_Realloc, Python_Free};
    PyMem_SetAllocator(PYMEM_DOMAIN_RAW, &alloc);
    PyMem_SetAllocator(PYMEM_DOMAIN_MEM, &alloc);
    PyMem_SetAllocator(PYMEM_DOMAIN_OBJ, &alloc);
}

