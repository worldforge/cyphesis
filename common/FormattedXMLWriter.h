// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include <Atlas/Message/Element.h>

#include <string>
#include <fstream>

/// Class to read old cyphesis rules on standard input, and output in new
/// standard format.
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
    void outputList(const Atlas::Message::Element::ListType &);

    /// Write the contents of this atlas map recursively to the stream.
    void outputMap(const Atlas::Message::Element::MapType &);
  public:
    /// Construct a writer object to write to the given ostream.
    explicit FormattedXMLWriter(std::ostream & stream);

    /// Write the atlas header to the stream.
    void openOutput();

    /// Write the atlas tail to the stream.
    void closeOutput();

    /// Write the contents of this top level object to the stream
    void outputObject(const Atlas::Message::Element::MapType & o);
};
