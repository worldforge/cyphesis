// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "Structure.h"

Structure::Structure(const std::string & id) : Structure_parent(id)
{
    m_location.setSimple(false);
}
