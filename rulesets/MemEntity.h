// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef MEM_ENTITY_H
#define MEM_ENTITY_H

#include "modules/Location.h"

#include <set>

class MemEntity;

typedef std::map<std::string, MemEntity *> MemEntityDict;
typedef std::vector<MemEntity *> MemEntityVector;
typedef std::set<MemEntity *> MemEntitySet;

class MemEntity {
  private:
    const std::string m_id;
  protected:
    Atlas::Message::Element::MapType m_attributes;
    std::string m_type;         // Easy access to primary parent
  public:
    Location m_location;        // Full details of location
    MemEntitySet m_contains;       // List of entities which use this as ref

    explicit MemEntity(const std::string & id);
    ~MemEntity();

    const std::string & getId() const { return m_id; }
    const std::string & getType() const { return m_type; }

    void setType(const std::string & t) {
        m_type = t;
    }

    const Atlas::Message::Element::MapType & getAttributes() const {
        return m_attributes;
    }

    bool get(const std::string &, Atlas::Message::Element &) const;
    void set(const std::string &, const Atlas::Message::Element &);

    void merge(const Atlas::Message::Element::MapType &);
    bool getLocation(const Atlas::Message::Element::MapType &,
                     const MemEntityDict &);
    Vector3D getXyz() const;

    void addToObject(Atlas::Message::Element::MapType & obj) const;
};

#endif // MEM_ENTITY_H
