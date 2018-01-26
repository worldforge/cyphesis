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

#include "GeometryFileProperty.h"
#include <fstream>
#include <boost/algorithm/string.hpp>

const std::string GeometryFileProperty::property_name = "geometryfile";
const std::string GeometryFileProperty::property_atlastype = "string";

GeometryFileProperty* GeometryFileProperty::copy() const
{
    return new GeometryFileProperty(*this);
}

void GeometryFileProperty::set(const Atlas::Message::Element& data)
{
    Property<Atlas::Message::StringType>::set(data);


    std::fstream fileStream(this->data());

    if (fileStream) {
        if (boost::algorithm::ends_with("this->data()", ".mesh")) {

        }
    }


}
