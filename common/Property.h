// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef COMMON_PROPERTY_H
#define COMMON_PROPERTY_H

#include <Atlas/Message/Element.h>

class PropertyBase {
  protected:
    const unsigned int m_flags;
    explicit PropertyBase(unsigned int);
  public:
    virtual ~PropertyBase();

    unsigned int flags() const { return m_flags; }

    virtual void get(Atlas::Message::Element &) = 0;
    virtual void set(const Atlas::Message::Element &) = 0;
    virtual void add(const std::string &, Atlas::Message::MapType & map);
};

template <typename T>
class Property : public PropertyBase {
  protected:
    T & m_data;
  public:
    explicit Property(T & data, unsigned int flags);

    virtual void get(Atlas::Message::Element &);
    virtual void set(const Atlas::Message::Element &);
    virtual void add(const std::string &, Atlas::Message::MapType & map);
};

template <typename T>
class ImmutableProperty : public PropertyBase {
  protected:
    const T & m_data;
  public:
    explicit ImmutableProperty(const T & data);

    virtual void get(Atlas::Message::Element &);
    virtual void set(const Atlas::Message::Element &);
    virtual void add(const std::string &, Atlas::Message::MapType & map);
};

#endif // COMMON_PROPERTY_H
