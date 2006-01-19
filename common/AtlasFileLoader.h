// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004-2005 Alistair Riddoch
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
