// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#include "FormattedXMLWriter.h"

#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

FormattedXMLWriter::FormattedXMLWriter(std::ostream & stream) :
                                       m_stream(stream), m_indent(0)
{
}

void FormattedXMLWriter::openOutput()
{
    m_stream << "<atlas>" << std::endl;
    m_indent += 2;
}

void FormattedXMLWriter::closeOutput()
{
    m_indent -= 2;
    m_stream << "</atlas>" << std::endl;
}

void FormattedXMLWriter::outputObject(const MapType & o)
{
    m_stream << "  <map>" << std::endl;
    m_indent += 2;
    outputMap(o);
    m_indent -= 2;
    m_stream << "  </map>" << std::endl << std::endl;
}

const char * FormattedXMLWriter::typeToStr(Element::Type t) const
{
    switch (t) {
        case Element::TYPE_INT:
            return "int";
        case Element::TYPE_FLOAT:
            return "float";
        case Element::TYPE_STRING:
            return "string";
        case Element::TYPE_MAP:
            return "map";
        case Element::TYPE_LIST:
            return "list";
        case Element::TYPE_NONE:
        default:
            return "NONE";
    }
}

void FormattedXMLWriter::outputValue(const Element & e)
{
    switch (e.getType()) {
        case Element::TYPE_INT:
            m_stream << e.Int();
            break;
        case Element::TYPE_FLOAT:
            m_stream << e.Float();
            break;
        case Element::TYPE_STRING:
            m_stream << e.String();
            break;
        case Element::TYPE_MAP:
            m_stream << std::endl;
            m_indent += 2;
            outputMap(e.Map());
            m_indent -= 2;
            for(int i = 0; i < m_indent; ++i) {
                m_stream << " ";
            }
            break;
        case Element::TYPE_LIST:
            m_stream << std::endl;
            m_indent += 2;
            outputList(e.List());
            m_indent -= 2;
            for(int i = 0; i < m_indent; ++i) {
                m_stream << " ";
            }
            break;
        case Element::TYPE_NONE:
        default:
            m_stream << e.Int();
            break;
    }
}

void FormattedXMLWriter::outputMap(const MapType & o)
{
    auto Iend = o.end();
    for (auto I = o.begin(); I != Iend; ++I) {
        for (int i = 0; i < m_indent; ++i) {
            m_stream << " ";
        }
        const char * type = typeToStr(I->second.getType());
        m_stream << "<" << type << " name=\""
                  << I->first << "\">";
        outputValue(I->second);
        m_stream << "</" << type << ">";
        m_stream << std::endl;
    }
}

void FormattedXMLWriter::outputList(const ListType & o)
{
    auto Iend = o.end();
    for(auto I = o.begin(); I != Iend; ++I) {
        for(int i = 0; i < m_indent; ++i) {
            m_stream << " ";
        }
        const char * type = typeToStr(I->getType());
        m_stream << "<" << type << ">";
        outputValue(*I);
        m_stream << "</" << type << ">";
        m_stream << std::endl;
    }
}
