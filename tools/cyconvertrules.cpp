// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "common/globals.h"

#include <Atlas/Message/DecoderBase.h>
#include <Atlas/Codecs/XML.h>

#include <string>
#include <fstream>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

/// Class to read old cyphesis rules on standard input, and output in new
/// standard format.
class FileConverter : public Atlas::Message::DecoderBase {
  private:
    /// Input file
    std::fstream m_file;
    /// Atlas codec for decoding input
    Atlas::Codecs::XML m_codec;
    /// Counter for rules read from input
    int m_count;
    /// Counter to keep track of indenting the output
    int m_indent;

    virtual void objectArrived(const Element & obj);
  public:
    FileConverter(const std::string & filename) :
                m_file(filename.c_str(), std::ios::in),
                m_codec(m_file, this), m_count(0), m_indent(0)
    {
    }

    /// Write the atlas header to standard out
    void openOutput() {
        std::cout << "<atlas>" << std::endl;
        m_indent = 4;
    }

    /// Write the atlas tail to standard out
    void closeOutput() {
        m_indent = 0;
        std::cout << "</atlas>" << std::endl;
    }

    /// Read input file to atlas codec.
    void read() {
        while (!m_file.eof()) {
            m_codec.poll();
        }
    }

    /// Report how many class we converted
    void report() {
        std::cerr << m_count << " classes read and converted."
                  << std::endl << std::flush;
    }

    /// Indicate if the input file has been opened successfully
    bool isOpen() {
        return m_file.is_open();
    }

    /// Write the contents of this element recursively to standard out
    void outputValue(const Element &);
    /// Write the contents of this atlas list recursively to standard out
    void output(const ListType &);
    /// Write the contents of this atlas map recursively to standard out
    void output(const MapType &);
    /// Return a character string corresponding to this atlas element type
    const char * typeToStr(Element::Type t) const;
};

void FileConverter::objectArrived(const Element & obj)
{
    // We have read a map from the file. Old format rules files contained
    // one map, which contained all the rules as maps within the top
    // level map. Iterate over them, and convert into new format;
    const MapType & omap = obj.asMap();
    MapType::const_iterator I;
    for (I = omap.begin(); I != omap.end(); ++I) {
        MapType newObject;
        // The id of the new rule is the key of this rule in the old map
        newObject["id"] = I->first;
        const MapType & ent = I->second.asMap();

        // Old format parent attribute was a single string. This is replaced
        // in the new format by the more conventional atlas list of strings.
        MapType::const_iterator J = ent.find("parent");
        if ((J == ent.end()) || (!J->second.isString())) {
            std::cerr << "Rule \"" << I->first << "\" has no parent."
                      << std::endl << std::flush;
            continue;
        }
        newObject["parents"] = ListType(1, J->second.asString());

        // Default attribute values, which straightforward values in the old
        // format, are converted to more complex structures defining
        // atlas attribute inheritance, server side visibility and default
        // value
        J = ent.find("attributes");
        if (J != ent.end()) {
            if (!J->second.isMap()) {
                std::cerr << "Rule \"" << I->first
                          << "\" has attributes which are not a map."
                          << std::endl << std::flush;
                continue;
            }
            newObject["attributes"] = MapType();
            MapType & newAttrs = newObject["attributes"].asMap();
            const MapType & attributes = J->second.asMap();
            J = attributes.begin();
            for(; J != attributes.end(); ++J) {
                MapType & attr = (newAttrs[J->first] = MapType()).asMap();
                attr["default"] = J->second;
                attr["visibility"] = "public";
            }
        }

        // The script attribute of the rule is converted unchanged, largely
        // because additional requirements have not yet been identified.
        J = ent.find("script");
        if (J != ent.end()) {
            if (!J->second.isMap()) {
                std::cerr << "Rule \"" << I->first
                          << "\" has script which are not a map."
                          << std::endl << std::flush;
                continue;
            }
            newObject["script"] = J->second;
        }

        // The mind attribute of the rule is converted unchanged, largely
        // because additional requirements have not yet been identified.
        J = ent.find("mind");
        if (J != ent.end()) {
            if (!J->second.isMap()) {
                std::cerr << "Rule \"" << I->first
                          << "\" has mind which are not a map."
                          << std::endl << std::flush;
                continue;
            }
            newObject["mind"] = J->second;
        }

        // The playable attribute of the rule is converted unchanged
        // Perhaps this should in time become an internal attribute?
        J = ent.find("playable");
        if (J != ent.end()) {
            if (!J->second.isInt()) {
                std::cerr << "Rule \"" << I->first
                          << "\" has playable which are not an int."
                          << std::endl << std::flush;
                continue;
            }
            newObject["playable"] = J->second;
        }

        // Output the newly converted rule, and continue.
        std::cout << "  <map>" << std::endl;
        output(newObject);
        std::cout << "  </map>" << std::endl << std::endl;
        ++m_count;
    }
}

const char * FileConverter::typeToStr(Element::Type t) const
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

void FileConverter::outputValue(const Element & e)
{
    switch (e.getType()) {
        case Element::TYPE_INT:
            std::cout << e.asInt();
            break;
        case Element::TYPE_FLOAT:
            std::cout << e.asFloat();
            break;
        case Element::TYPE_STRING:
            std::cout << e.asString();
            break;
        case Element::TYPE_MAP:
            std::cout << std::endl;
            m_indent += 2;
            output(e.asMap());
            m_indent -= 2;
            for(int i = 0; i < m_indent; ++i) {
                std::cout << " ";
            }
            break;
        case Element::TYPE_LIST:
            std::cout << std::endl;
            m_indent += 2;
            output(e.asList());
            m_indent -= 2;
            for(int i = 0; i < m_indent; ++i) {
                std::cout << " ";
            }
            break;
        case Element::TYPE_NONE:
        default:
            std::cout << e.asInt();
            break;
    };
}

void FileConverter::output(const MapType & o)
{
    for(MapType::const_iterator I = o.begin(); I != o.end(); ++I) {
        for(int i = 0; i < m_indent; ++i) {
            std::cout << " ";
        }
        const char * type = typeToStr(I->second.getType());
        std::cout << "<" << type << " name=\""
                  << I->first << "\">";
        outputValue(I->second);
        std::cout << "</" << type << ">";
        std::cout << std::endl;
    }
}

void FileConverter::output(const ListType & o)
{
    for(ListType::const_iterator I = o.begin(); I != o.end(); ++I) {
        for(int i = 0; i < m_indent; ++i) {
            std::cout << " ";
        }
        const char * type = typeToStr(I->getType());
        std::cout << "<" << type << ">";
        outputValue(*I);
        std::cout << "</" << type << ">";
        std::cout << std::endl;
    }
}

static void usage(char * prgname)
{
    std::cerr << "usage: " << prgname << " <old rule file>"
              << std::endl << std::flush;
    return;
}

int main(int argc, char ** argv)
{
    // We require the user to specify the input file
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    // Attempt to open input
    FileConverter f(argv[1]);
    if (!f.isOpen()) {
        std::cerr << "ERROR: Unable to open file " << argv[1]
                  << std::endl << std::flush;
        return 1;
    }
    // Output header elements
    f.openOutput();
    // Read input file, which will be converted on the fly and copied
    // to standard out.
    f.read();
    // Output tail elements
    f.closeOutput();
    // Report how many rules converted to stderr
    f.report();
}
