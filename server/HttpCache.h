// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2008 Alistair Riddoch
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

#ifndef SERVER_HTTP_CACHE_H
#define SERVER_HTTP_CACHE_H

#include <list>
#include <string>

/// \brief A caching generator for the results of http requests.
///
class HttpCache {
  private:
    HttpCache();
    static HttpCache * m_instance;

  protected:
    void sendHeaders(std::ostream &,
                     int status = 200,
                     const std::string & mesg = "OK");

    void reportBadRequest(std::ostream &,
                          int status = 400,
                          const std::string & mesg = "Bad Request");
  public:
    static HttpCache * instance() {
        if (m_instance == NULL) {
            m_instance = new HttpCache();
        }
        return m_instance;
    }
    static void del() {
        if (m_instance != NULL) {
            delete m_instance;
        }
    }
    void processQuery(std::ostream &, const std::list<std::string> &);
};

#endif // SERVER_HTTP_CACHE_H
