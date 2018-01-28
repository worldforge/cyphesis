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
#include "common/globals.h"
#include "common/log.h"
#include "common/TypeNode.h"
#include "OgreMeshDeserializer.h"
#include "GeometryProperty.h"
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

    try {
        if (boost::algorithm::ends_with(this->data(), ".mesh")) {
            std::string path = share_directory + "/cyphesis/assets/" + this->data();
            std::fstream fileStream(path);
            if (fileStream) {
                OgreMeshDeserializer deserializer(fileStream);
                deserializer.deserialize();

                m_indices = std::move(deserializer.m_indices);
                m_vertices = std::move(deserializer.m_vertices);
                m_bounds = deserializer.m_bounds;
                m_radius = deserializer.m_radius;
            } else {
                log(ERROR, "Could not find geometry file at " + path);
            }
        } else {
            log(ERROR, "Could not recognize geometry file type: " + this->data());
        }
    } catch (const std::exception& ex) {
        log(ERROR, "Exception when trying to parse geometry at " + this->data());
    }


}

void GeometryFileProperty::install(TypeNode* typeNode, const std::string& name)
{
    Atlas::Message::MapType geometryData;
    geometryData["shape"] = "mesh";

    Atlas::Message::ListType indices;
    indices.reserve(m_indices.size());
    std::copy(m_indices.begin(), m_indices.end(), std::back_inserter(indices));
    geometryData["indices"] = std::move(indices);

    Atlas::Message::ListType vertices;
    vertices.reserve(m_vertices.size());
    std::copy(m_vertices.begin(), m_vertices.end(), std::back_inserter(vertices));
    geometryData["vertices"] = std::move(vertices);


    auto I = typeNode->defaults().find(GeometryProperty::property_name);
    GeometryProperty* geometryProperty;
    if (I == typeNode->defaults().end()) {
        geometryProperty = new GeometryProperty();
        geometryProperty->setFlags(flag_class);
        geometryProperty->set(geometryData);
        typeNode->injectProperty(GeometryProperty::property_name, geometryProperty);
        geometryProperty->install(typeNode, GeometryProperty::property_name);
    } else {
        geometryProperty = dynamic_cast<GeometryProperty*>(I->second);
        geometryProperty->set(geometryData);
    }

}

void GeometryFileProperty::apply(LocatedEntity*)
{
    if ((this->flags() | flag_class) == 0) {
        //TODO: apply entity property
    }
}
