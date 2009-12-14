// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

// $Id$

#ifndef TOOLS_MULTI_LINE_LIST_FORMATTER_H
#define TOOLS_MULTI_LINE_LIST_FORMATTER_H

#include <Atlas/Formatter.h>

class MultiLineListFormatter : public Atlas::Formatter
{
  public:
    MultiLineListFormatter(std::iostream & s, Atlas::Bridge & b);

    virtual void mapListItem(const std::string & name);

    virtual void listMapItem();
    virtual void listListItem();
    virtual void listIntItem(long l);
    virtual void listFloatItem(double d);
    virtual void listStringItem(const std::string&s);
    virtual void listEnd();

};

#endif // TOOLS_MULTI_LINE_LIST_FORMATTER_H
