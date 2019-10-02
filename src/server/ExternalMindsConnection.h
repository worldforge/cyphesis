/*
 Copyright (C) 2013 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef EXTERNALMINDSCONNECTION_H_
#define EXTERNALMINDSCONNECTION_H_

#include <string>

class Link;

class ExternalMindsConnection
{
    public:
        explicit ExternalMindsConnection(Link* link,
                const std::string& routerId);
        ~ExternalMindsConnection();

        Link* getLink() const;
        const std::string& getRouterId() const;

    private:

        Link* m_link;
        std::string m_routerId;
};

#endif /* EXTERNALMINDSCONNECTION_H_ */
