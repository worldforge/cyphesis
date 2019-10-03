/*
 Copyright (C) 2018 Erik Ogenvik

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

#ifndef CYPHESIS_CYPY_MEMENTITY_H
#define CYPHESIS_CYPY_MEMENTITY_H


#include "rules/ai/MemEntity.h"
#include "rules/python/CyPy_LocatedEntity_impl.h"

/**
 * \ingroup PythonWrappers
 */
class CyPy_MemEntity : public CyPy_LocatedEntityBase<Ref<MemEntity>, CyPy_MemEntity>
{
    public:
        CyPy_MemEntity(Py::PythonClassInstanceWeak* self, Py::Tuple& args, Py::Dict& kwds);
        CyPy_MemEntity(Py::PythonClassInstanceWeak* self, Ref<MemEntity> value);

        ~CyPy_MemEntity() override;

        static void init_type();


    protected:

};

#endif //CYPHESIS_CYPY_MEMENTITY_H
