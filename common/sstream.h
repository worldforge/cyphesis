// <!!-*-C++-*- file: sstream.h --->
// <!!-------------------------------------------------------------------------->
// <!! Copyright (C) 1996 Dietmar Kuehl >
// <!!   Universitaet Konstanz, Lehrstuhl fuer praktische Informatik I >
// <!!>
// <!! Permission to use, copy, modify, distribute and sell this >
// <!! software for any purpose is hereby granted without fee, provided >
// <!! that the above copyright notice appears in all copies and that >
// <!! both that copyright notice and this permission notice appear in >
// <!! supporting documentation. Dietmar Kuehl makes no representations >
// <!! about the suitability of this software for any purpose. It is >
// <!! provided "as is" without express or implied warranty. >
// <!!-------------------------------------------------------------------------->

// Author: Dietmar Kühl dietmar.kuehl@uni-konstanz.de www.informatik.uni-konstanz.de/~kuehl
// Title:  An implementation of the string stream classes

//------------------------------------------------------------------------------
// The first thing to do is to protect the file against multiple inclusion:

#if !defined(SSTREAM_H)
#define SSTREAM_H

//------------------------------------------------------------------------------
// Next, some headerfiles are included:

#include <string>
#include <streambuf.h>
#include <iostream.h>
#include "iotraits.h"
#include <algorithm>

//------------------------------------------------------------------------------

#if defined(__GNUC__)
// NOTE: This hack will work for character type 'char' only!
// However, the actual code was taken from a fully templatized code base.

  template <class charT, class traits>
  class basic_streambuf: public streambuf
  {
  public:
    typedef char      char_type;
    typedef int       int_type;
    typedef ios_traits<char>    traits_type;
    typedef streampos pos_type;
    typedef streamoff off_type;
  };

  template <class charT, class traits>
  class basic_istream: public istream
  {
  public:
    typedef char      char_type;
    typedef int       int_type;
    typedef ios_traits<char>    traits_type;
    typedef streampos pos_type;
    typedef streamoff off_type;
  };
  template <class charT, class traits>
  class basic_ostream: public ostream
  {
  public:
    typedef char      char_type;
    typedef int       int_type;
    typedef ios_traits<char>    traits_type;
    typedef streampos pos_type;
    typedef streamoff off_type;
  };
  template <class charT, class traits>
  class basic_iostream: public iostream
  {
  public:
    basic_iostream(basic_streambuf<charT, traits>* sbuf = 0):
      iostream(sbuf) {}
    typedef char      char_type;
    typedef int       int_type;
    typedef ios_traits<char>    traits_type;
    typedef streampos pos_type;
    typedef streamoff off_type;
  };

  struct ios_base: public ios
  {
    typedef seek_dir seekdir;
  };
#endif

//------------------------------------------------------------------------------

template <class _CS_cT, class _CS_traits, class _CS_alloc = string_char_traits<_CS_cT> >
class basic_stringbuf: public basic_streambuf<_CS_cT, _CS_traits>
{
public:
  typedef basic_string<_CS_cT, _CS_traits, _CS_alloc> string_type;
  typedef typename basic_streambuf<_CS_cT, _CS_traits>::char_type char_type;
  typedef typename basic_streambuf<_CS_cT, _CS_traits>::traits_type traits_type;
  typedef typename basic_streambuf<_CS_cT, _CS_traits>::int_type int_type;
  typedef typename basic_streambuf<_CS_cT, _CS_traits>::pos_type pos_type;
  typedef typename basic_streambuf<_CS_cT, _CS_traits>::off_type off_type;

  basic_stringbuf(ios_base::openmode = ios_base::in | ios_base::out);
  basic_stringbuf(basic_string<_CS_cT, _CS_traits, _CS_alloc> const&,
                  ios_base::openmode = ios_base::in | ios_base::out);
  ~basic_stringbuf();

  basic_string<_CS_cT, _CS_traits, _CS_alloc> str() const;
  void str(basic_string<_CS_cT, _CS_traits, _CS_alloc> const&);

protected:
  int_type   underflow();
  int_type   pbackfail(int_type);
  int_type   overflow(int_type);
  basic_streambuf<_CS_cT, _CS_traits>* setbuf(char_type*, streamsize);
  pos_type   seekoff(off_type, ios_base::seekdir, ios_base::openmode);
  pos_type   seekpos(pos_type pos, ios_base::openmode);
  int sync();

private:
  basic_stringbuf(basic_stringbuf const&); // deliberatly not accessible
  void operator=(basic_stringbuf const&);  // deliberatly not accessible

  ios_base::openmode _CS_m_mode;
  _CS_cT*      _CS_m_beg;
  _CS_cT*      _CS_m_end;
};

// --------------------------------------------------------------------------

template <class _CS_cT, class _CS_Tr, class _CS_Al>
class _CS_basic_sstream_base
{
 public:
  typedef basic_string<_CS_cT, _CS_Tr, _CS_Al> _CS_string_type;

  explicit _CS_basic_sstream_base(ios_base::openmode);
  _CS_basic_sstream_base(_CS_string_type const&, ios_base::openmode);

  _CS_string_type str() const;
  void                  str(_CS_string_type const&);

 protected:
  mutable basic_stringbuf<_CS_cT, _CS_Tr, _CS_Al> _CS_m_ssbuf;
};

// --------------------------------------------------------------------------

template <class _CS_cT, class _CS_Tr, class _CS_Al>
class basic_istringstream: public virtual _CS_basic_sstream_base<_CS_cT, _CS_Tr, _CS_Al>,
                           public basic_istream<_CS_cT, _CS_Tr>
{
 public:
  typedef typename basic_istream<_CS_cT, _CS_Tr>::char_type char_type;
  typedef typename basic_istream<_CS_cT, _CS_Tr>::int_type int_type;
  typedef typename basic_istream<_CS_cT, _CS_Tr>::pos_type pos_type;
  typedef typename basic_istream<_CS_cT, _CS_Tr>::off_type off_type;
  typedef typename basic_istream<_CS_cT, _CS_Tr>::traits_type traits_type;
  typedef basic_string<_CS_cT, _CS_Tr, _CS_Al> _CS_string_type;

  explicit basic_istringstream(ios_base::openmode = ios_base::in);
  explicit basic_istringstream(_CS_string_type const&, ios_base::openmode = ios_base::in);

  basic_stringbuf<_CS_cT, _CS_Tr, _CS_Al>* rdbuf() const { return &_CS_m_ssbuf; }
};

// --------------------------------------------------------------------------

template <class _CS_cT, class _CS_Tr, class _CS_Al>
class basic_ostringstream: public virtual _CS_basic_sstream_base<_CS_cT, _CS_Tr, _CS_Al>,
                           public basic_ostream<_CS_cT, _CS_Tr>
{
 public:
  typedef typename basic_ostream<_CS_cT, _CS_Tr>::char_type char_type;
  typedef typename basic_ostream<_CS_cT, _CS_Tr>::int_type int_type;
  typedef typename basic_ostream<_CS_cT, _CS_Tr>::pos_type pos_type;
  typedef typename basic_ostream<_CS_cT, _CS_Tr>::off_type off_type;
  typedef basic_string<_CS_cT, _CS_Tr, _CS_Al> _CS_string_type;

  explicit basic_ostringstream(ios_base::openmode = ios_base::out);
  explicit basic_ostringstream(_CS_string_type const&, ios_base::openmode = ios_base::out);

  basic_stringbuf<_CS_cT, _CS_Tr, _CS_Al>* rdbuf() const { return &_CS_m_ssbuf; }
};

// --------------------------------------------------------------------------

template <class _CS_cT, class _CS_Tr, class _CS_Al>
class basic_stringstream: public virtual _CS_basic_sstream_base<_CS_cT, _CS_Tr, _CS_Al>,
                          public basic_iostream<_CS_cT, _CS_Tr>
{
 public:
  typedef basic_string<_CS_cT, _CS_Tr, _CS_Al> _CS_string_type;

  explicit basic_stringstream(ios_base::openmode = ios_base::in | ios_base::out);
  explicit basic_stringstream(_CS_string_type const&, ios_base::openmode = ios_base::in|ios_base::out);

  basic_stringbuf<_CS_cT, _CS_Tr, _CS_Al>* rdbuf() const { return &_CS_m_ssbuf; }
};

typedef basic_stringbuf<char, string_char_traits<char>, alloc>      stringbuf;
typedef basic_istringstream<char, string_char_traits<char>, alloc>  istringstream;
typedef basic_ostringstream<char, string_char_traits<char>, alloc>  ostringstream;
typedef basic_stringstream<char, string_char_traits<char>, alloc> stringstream;

//------------------------------------------------------------------------------

#if !defined(SSTREAM_CC)
#  include "sstream.cc"
#endif

#endif /* SSTREAM_H */
