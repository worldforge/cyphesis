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


#ifndef COMMON_FORMATTED_XML_WRITER_H
#define COMMON_FORMATTED_XML_WRITER_H

#include <Atlas/Message/Element.h>

#include <iosfwd>

/// \brief Class to output Atlas data in human readable formatted XML-like.
class FormattedXMLWriter {
  private:
    /// Output file
    std::ostream & m_stream;

    /// Counter to keep track of indenting the output
    int m_indent;

    /// Return a character string corresponding to this atlas element type.
    const char * typeToStr(Atlas::Message::Element::Type t) const;

    /// Write the contents of this element recursively to the stream.
    void outputValue(const Atlas::Message::Element &);

    /// Write the contents of this atlas list recursively to the stream.
    void outputList(const Atlas::Message::ListType &);

    /// Write the contents of this atlas map recursively to the stream.
    void outputMap(const Atlas::Message::MapType &);
  public:
    /// Construct a writer object to write to the given ostream.
    explicit FormattedXMLWriter(std::ostream & stream);

    /// Write the atlas header to the stream.
    void openOutput();

    /// Write the atlas tail to the stream.
    void closeOutput();

    /// Write the contents of this top level object to the stream
    void outputObject(const Atlas::Message::MapType & o);
};

#endif // COMMON_FORMATTED_XML_WRITER_H
