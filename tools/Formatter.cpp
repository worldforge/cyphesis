// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "Formatter.h"

#include <iostream>

Formatter::Formatter(std::iostream & s, Atlas::Bridge & b) : m_stream(s),
                                                             m_bridge(b),
                                                             m_indent(0),
                                                             m_spacing(2)
{
}

void Formatter::streamBegin()
{
    m_bridge.streamBegin();
    m_indent = m_spacing;
    m_stream << std::endl;
}

void Formatter::streamMessage(const Atlas::Bridge::Map&m)
{
    m_stream << std::endl;
    m_stream << std::string(m_indent, ' ');
    m_bridge.streamMessage(m);
    m_indent += m_spacing;
    m_stream << std::endl;
}

void Formatter::streamEnd()
{
    m_stream << std::endl;
    m_bridge.streamEnd();
    m_stream << std::endl;
}

    
void Formatter::mapItem(const std::string & name, const Atlas::Bridge::Map&m)
{
    m_stream << std::string(m_indent, ' ');
    m_bridge.mapItem(name,m);
    m_indent += m_spacing;
    m_stream << std::endl;
}

void Formatter::mapItem(const std::string & name, const Atlas::Bridge::List&l)
{
    m_stream << std::string(m_indent, ' ');
    m_bridge.mapItem(name,l);
    m_indent += m_spacing;
    m_stream << std::endl;
}

void Formatter::mapItem(const std::string & name, long l)
{
    m_stream << std::string(m_indent, ' ');
    m_bridge.mapItem(name, l);
    m_stream << std::endl;
}

void Formatter::mapItem(const std::string & name, double d)
{
    m_stream << std::string(m_indent, ' ');
    m_bridge.mapItem(name, d);
    m_stream << std::endl;
}

void Formatter::mapItem(const std::string & name, const std::string & s)
{
    m_stream << std::string(m_indent, ' ');
    m_bridge.mapItem(name, s);
    m_stream << std::endl;
}

void Formatter::mapEnd()
{
    m_indent -= m_spacing;
    m_stream << std::string(m_indent, ' ');
    m_bridge.mapEnd();
    m_stream << std::endl;
}

    
void Formatter::listItem(const Atlas::Bridge::Map&m)
{
    m_stream << std::string(m_indent, ' ');
    m_bridge.listItem(m);
    m_indent += m_spacing;
    m_stream << std::endl;
}

void Formatter::listItem(const Atlas::Bridge::List&l)
{
    m_stream << std::string(m_indent, ' ');
    m_bridge.listItem(l);
    m_indent += m_spacing;
    m_stream << std::endl;
}

void Formatter::listItem(long l)
{
    m_stream << std::string(m_indent, ' ');
    m_bridge.listItem(l);
    m_stream << std::endl;
}

void Formatter::listItem(double d)
{
    m_stream << std::string(m_indent, ' ');
    m_bridge.listItem(d);
    m_stream << std::endl;
}

void Formatter::listItem(const std::string & s)
{
    m_stream << std::string(m_indent, ' ');
    m_bridge.listItem(s);
    m_stream << std::endl;
}

void Formatter::listEnd()
{
    m_indent -= m_spacing;
    m_stream << std::string(m_indent, ' ');
    m_bridge.listEnd();
    m_stream << std::endl;
}
