// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2012 Alistair Riddoch
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

#ifndef TESTS_NULL_STREAM_H
#define TESTS_NULL_STREAM_H

class null_stream {
  private:
    bool m_fail_val;
    int m_peek_val;

    bool m_timeout_val;
    int m_socket_val;
    bool m_is_open_val;
  public:
    void reset_fail_val(bool v) { m_fail_val = v; }
    void reset_peek_val(int v) { m_peek_val = v; }

    void reset_timeout_val(bool v) { m_timeout_val = v; }
    void reset_socket_val(int v) { m_socket_val = v; }
    void reset_is_open_val(bool v) { m_is_open_val = v; }


    class traits_type {
      public:
        static int eof() { return -1; }
    };

    null_stream() { }
    explicit null_stream(int fd) : m_socket_val(fd) { }

    int peek() { return m_peek_val; }
    bool fail() const { return m_fail_val; }
    void setstate(int) { }
    void flush() { }

    bool timeout() { return m_timeout_val; }
    void shutdown(bool = false) { }
    int getSocket() const { return m_socket_val; }
    bool is_open() const { return m_is_open_val; }
};

#endif // TESTS_NULL_STREAM_H
