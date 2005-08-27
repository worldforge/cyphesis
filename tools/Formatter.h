// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#error This file has been removed from the build

#ifndef TOOLS_FORMATTER_H
#define TOOLS_FORMATTER_H

#include <Atlas/Bridge.h>

class Formatter : public Atlas::Bridge
{
  public:
    
    Formatter(std::iostream & s, Atlas::Bridge & b);

    virtual void streamBegin();
    virtual void streamMessage(const Atlas::Bridge::Map &);
    virtual void streamEnd();
    
    virtual void mapItem(const std::string& name, const Atlas::Bridge::Map&);
    virtual void mapItem(const std::string& name, const Atlas::Bridge::List&);
    virtual void mapItem(const std::string& name, long);
    virtual void mapItem(const std::string& name, double);
    virtual void mapItem(const std::string& name, const std::string&);
    virtual void mapEnd();
    
    virtual void listItem(const Atlas::Bridge::Map&);
    virtual void listItem(const Atlas::Bridge::List&);
    virtual void listItem(long);
    virtual void listItem(double);
    virtual void listItem(const std::string&);
    virtual void listEnd();

    void setSpacing(int s) {
        m_spacing = s;
    }

  protected:

    std::iostream & m_stream;
    Bridge & m_bridge;
    int m_indent;
    int m_spacing;
};

#endif // TOOLS_FORMATTER_H
