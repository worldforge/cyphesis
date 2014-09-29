/*
 Copyright (C) 2014 Erik Ogenvik

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
#ifndef STUBSYSTEMACCOUNT_H_
#define STUBSYSTEMACCOUNT_H_

SystemAccount::SystemAccount(Connection * conn,
                             const std::string & username,
                             const std::string & passwd,
                             const std::string & id, long intId) :
               Admin(conn, username, passwd, id, intId)
{
}

SystemAccount::~SystemAccount()
{
}

const char * SystemAccount::getType() const
{
    return "sys";
}

bool SystemAccount::isPersisted() const {
    return false;
}



#endif /* STUBSYSTEMACCOUNT_H_ */
