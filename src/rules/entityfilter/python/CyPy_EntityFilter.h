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

#ifndef CYPHESIS_CYPY_ENTITYFILTER_H
#define CYPHESIS_CYPY_ENTITYFILTER_H

#include <rules/entityfilter/Filter.h>
#include "pycxx/CXX/Objects.hxx"
#include "pycxx/CXX/Extensions.hxx"
#include "pythonbase/WrapperBase.h"
#include <memory>


/**
 * \ingroup PythonWrappers
 */
class CyPy_Filter : public WrapperBase<std::shared_ptr<EntityFilter::Filter>, CyPy_Filter>
{
    public:

        CyPy_Filter(Py::PythonClassInstance* self, Py::Tuple& args, Py::Dict& kwds);
        CyPy_Filter(Py::PythonClassInstance* self, std::shared_ptr<EntityFilter::Filter> value);

        static void init_type();

        Py::Object str() override;

    private:

};

/**
 * \ingroup PythonWrappers
 */
class CyPy_EntityFilter : public Py::ExtensionModule<CyPy_EntityFilter>
{
    public:
        CyPy_EntityFilter();
        static std::string init();

};


#endif //CYPHESIS_CYPY_ENTITYFILTER_H
