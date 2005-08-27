// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004-2005 Alistair Riddoch

#ifndef COMMON_ATLAS_FILE_LOADER_H
#define COMMON_ATLAS_FILE_LOADER_H

#include <Atlas/Message/DecoderBase.h>
#include <Atlas/Codecs/XML.h>

#include <fstream>

/// Class to read old cyphesis rules on standard input, and output in new
/// standard format.
class AtlasFileLoader : public Atlas::Message::DecoderBase {
  private:
    /// Input file
    std::fstream m_file;
    /// Atlas codec for decoding input FIXME Make this generic
    Atlas::Codecs::XML m_codec;
    /// Counter for messages read from input
    int m_count;
    /// Store for the messages loaded
    Atlas::Message::MapType & m_messages;

    virtual void messageArrived(const Atlas::Message::MapType & msg);
  public:
    AtlasFileLoader(const std::string & filename, Atlas::Message::MapType & m);

    /// Indicate if the input file has been opened successfully
    bool isOpen() {
        return m_file.is_open();
    }

    /// Read input file to atlas codec.
    void read() {
        while (!m_file.eof()) {
            m_codec.poll();
        }
    }

    int count() { return m_count; }
};

#endif // COMMON_ATLAS_FILE_LOADER_H
