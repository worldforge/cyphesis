// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "FormattedXMLWriter.h"

using Atlas::Message::Element;
typedef Element::MapType MapType;
typedef Element::ListType ListType;

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

void FormattedXMLWriter::outputObject(const Element::MapType & o)
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
    };
}

void FormattedXMLWriter::outputValue(const Element & e)
{
    switch (e.getType()) {
        case Element::TYPE_INT:
            m_stream << e.asInt();
            break;
        case Element::TYPE_FLOAT:
            m_stream << e.asFloat();
            break;
        case Element::TYPE_STRING:
            m_stream << e.asString();
            break;
        case Element::TYPE_MAP:
            m_stream << std::endl;
            m_indent += 2;
            outputMap(e.asMap());
            m_indent -= 2;
            for(int i = 0; i < m_indent; ++i) {
                m_stream << " ";
            }
            break;
        case Element::TYPE_LIST:
            m_stream << std::endl;
            m_indent += 2;
            outputList(e.asList());
            m_indent -= 2;
            for(int i = 0; i < m_indent; ++i) {
                m_stream << " ";
            }
            break;
        case Element::TYPE_NONE:
        default:
            m_stream << e.asInt();
            break;
    };
}

void FormattedXMLWriter::outputMap(const MapType & o)
{
    for(MapType::const_iterator I = o.begin(); I != o.end(); ++I) {
        for(int i = 0; i < m_indent; ++i) {
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
    for(ListType::const_iterator I = o.begin(); I != o.end(); ++I) {
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
