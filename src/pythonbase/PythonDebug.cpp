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

#include "PythonDebug.h"

#include "pycxx/CXX/Objects.hxx"

void pythonStackTrace() {
    Py::Module sys("sys");
    Py::Module traceback("traceback");
    auto currentFrames = Py::Dict(sys.callMemberFunction("_current_frames"));
    for (auto entry : currentFrames) {
        traceback.callMemberFunction("print_stack", Py::TupleN(entry.second));
    }
}
