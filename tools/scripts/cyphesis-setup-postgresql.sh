#!/bin/sh
# Cyphesis Online RPG Server and AI Engine
# Copyright (C) 2002-2007 Alistair Riddoch
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


# The main purpose of this script is to get round the issue of access to
# to the database required by cyphesis.

cat << EOWELCOME
Welcome to cyphesis.

This script will go through the steps required to configure
PostgreSQL so that you can run cyphesis. This script
must be run as root or with sudo access. It is strongly recommended that
you run cyphesis using a normal user account, and this script will help to
set up access for this account to the database.

EOWELCOME

SCRIPTDIR=${PWD}/`dirname $0`

cd /

# Check who is running this script. This should either be the user who is
# going to run cyphesis, or root if extra privs are required to set up
# database access
RUNNINGAS=`whoami`

if test ${RUNNINGAS} != "root"; then
    echo Not running as root, so will try and use sudo.
    echo 
    if ! sudo -p "Please enter your password for sudo access or hit ctrl-c to abort:" /bin/true; then
        echo ERROR: $0:
        echo You must run $0 as root in order to create the database accounts.
        echo The best way to run it is using sudo or su.
        exit 1
    fi
fi

if test x${USERNAME} != "x" && test ${USERNAME} != "root"; then
    true
    # USERNAME=${USERNAME}
elif test x${SUDO_USER} != "x" && test ${SUDO_USER} != "root"; then
    USERNAME=${SUDO_USER}
elif test x${USER} != "x" && test ${USER} != "root"; then
    USERNAME=${USER}
elif test x${LOGNAME} != "x" && test ${LOGNAME} != "root"; then
    USERNAME=${LOGNAME}
fi

while test x${USERNAME} == "x" || test ${USERNAME} == "root"; do
    echo
    echo Unable to determine your real username.
    echo Cyphesis cannot be run as root for security reasons.
    echo Please give the username of the account that will be running cyphesis,
    echo or hit ctrl-c to abort.
    read -e -p "Your username:" REALUSERNAME

    if sudo su ${REALUSERNAME} -c /bin/true; then
        USERNAME=${REALUSERNAME}
    else
        echo
        echo Unable to become account ${USERNAME}.
        echo It looks as though you did not enter the username of a valid account.
    fi
done

# exit 0

# We now have root privs using sudo, and know the users real username in
# ${USERNAME}. We can proceed to set up their account.

CREATE_USER_CMD=`which createuser 2>/dev/null`
CREATE_USER_FLAGS='-d -w -S -R'
CREATE_DATABASE_CMD=`which createdb 2>/dev/null`
DB_SUSER=postgres
DB_QUERY_CMD=`which psql 2>/dev/null`
DB_TEMPLATE=template1

# Check whether the user already has an account. There is more than one
# reason why this might fail, but we assume if it does that it
# means the user does not have access. The database account we create
# has the right to create databases.
if sudo -u ${USERNAME} ${DB_QUERY_CMD} -c "" ${DB_TEMPLATE} 2> /dev/null ; then
    echo PostgreSQL user ${USERNAME} already exists. Good.
else
    echo Creating PostgreSQL account for user ${USERNAME}...
    if sudo -u ${DB_SUSER} ${CREATE_USER_CMD} ${CREATE_USER_FLAGS} ${USERNAME} ; then
      echo Created PostgreSQL user ${USERNAME}.
    else
      echo ERROR: $0: Unable to create database account ${USERNAME}.
      exit 1
    fi
fi

echo

# Check whether a database called cyphesis exists, and the user we are
# setting up can connect to it. If the database exists, but we cannot
# connect to it, this will fail. If the user already had a PostgeSQL
# account, but did not have the right to create databases, this will fail.
if sudo -u ${USERNAME} $DB_QUERY_CMD -c "" cyphesis 2> /dev/null ; then
    echo PostgreSQL database cyphesis already exists. Good.
else
    echo Creating PostgreSQL database cyphesis as user ${USERNAME}...
    if sudo -u ${USERNAME} ${CREATE_DATABASE_CMD} cyphesis ; then
      echo Created PostgreSQL database cyphesis as ${USERNAME}.
    else
      echo ERROR: $0: Unable to create database cyphesis.
      exit 1
    fi
fi


echo
echo This system is now ready to run cyphesis.
