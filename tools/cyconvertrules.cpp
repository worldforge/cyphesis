// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "common/globals.h"

#include <Atlas/Message/DecoderBase.h>
#include <Atlas/Codecs/XML.h>

#include <string>
#include <fstream>

using Atlas::Message::Element;

class FileConverter : public Atlas::Message::DecoderBase {
    std::fstream m_file;
    Atlas::Codecs::XML m_codec;
    int m_count;
    int m_indent;

    virtual void objectArrived(const Element & obj);
  public:
    FileConverter(const std::string & filename) :
                m_file(filename.c_str(), std::ios::in),
                m_codec(m_file, this), m_count(0), m_indent(0)
    {
    }

    void openOutput() {
        std::cout << "<atlas>" << std::endl << "  <map>" << std::endl;
        m_indent = 4;
    }

    void closeOutput() {
        m_indent = 0;
        std::cout << "  <map>" << std::endl << "<atlas>" << std::endl;
    }

    void read() {
        while (!m_file.eof()) {
            m_codec.poll();
        }
    }

    void report() {
        std::cerr << m_count << " classes read and converted."
                  << std::endl << std::flush;
    }

    bool isOpen() {
        return m_file.is_open();
    }
};

void FileConverter::objectArrived(const Element & obj)
{
    const Element::MapType & omap = obj.asMap();
    Element::MapType::const_iterator I;
    for (I = omap.begin(); I != omap.end(); ++I) {
        ++m_count;
        Element::MapType newObject;
        newObject["id"] = I->first;
        const Element::MapType & ent = I->second.asMap();
        Element::MapType::const_iterator J = ent.find("parent");
        if ((J == ent.end()) || (!J->second.isString())) {
            std::cerr << "Rule \"" << I->first << "\" has no parent"
                      << std::endl << std::flush;
        }
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
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    FileConverter f(argv[1]);
    if (!f.isOpen()) {
        std::cerr << "ERROR: Unable to open file " << argv[1]
                  << std::endl << std::flush;
        return 1;
    }
    f.openOutput();
    f.read();
    f.closeOutput();
    f.report();
}
