/*
    Generic Game Metaserver

    Copyright (C) 2000 Dragon Master

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as
    published by the Free Software Foundation.  A copy of this license is
    included in this package.  If there is any discrepancy between this copy
    and the version published by the Free Software Foundation, the included
    version shall apply.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    The author can be reached via e-mail to dragonm@leech.org
*/
#error This file has been removed from the build
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#ifndef PROTOCOL_INSTRUCTIONS_H
#define PROTOCOL_INSTRUCTIONS_H

#define SKEEP_ALIVE (long)1
#define CKEEP_ALIVE (long)2
#define HANDSHAKE (long)3
#define SERVERSHAKE (long)4
#define CLIENTSHAKE (long)5
#define TERMINATE (long)6
#define LIST_REQ (long)7
#define LIST_RESP (long)8
#define PROTO_ERANGE (long)9

#endif
