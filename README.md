# Cyphesis, the Worldforge server

[![Join us on Gitter!](https://badges.gitter.im/Worldforge.svg)](https://gitter.im/Worldforge/Lobby)

Cyphesis is the server for the [WorldForge](http://worldforge.org/ "The main Worldforge site") system.

## Installation

If you intend to build this as a prerequisite for the Ember client or the Cyphesis server we strongly suggest that you 
use the [Hammer](http://wiki.worldforge.org/wiki/Hammer_Script "The Hammer script") tool to compile Ember.
This is script provided by the Worldforge project which will download and install all of the required libraries and 
components used by Worldforge.

Otherwise the library can most easily be built through the following commands.
```
mkdir build_`arch` && cd build_`arch`
cmake ..
make
make install
```

### Tests

The test suite can be built and run using the ```check``` target. For example:

```
make check
```

### API documentation

If Doxygen is available API documentation can be generated using the ```docs``` target. For example:

```
make docs
```

## Dependencies

Cyphesis is built using CMake. 
It requires Python and PostgreSQL which are included with most Linux
distributions, and Atlas-C++, varconf, Mercator and wfmath 
which are provided by the WorldForge project. GNU readline is required by
some of the included tools.
Boost is used as well, mainly for the ASIO library which drives networking.

If built from source the software and data must be installed using "make
install" before it will be ready. Go to the Section called Setup for
information on the setup steps required after installation.


## Setup

The software requires some post-installation configuration before it will
run correctly. If you plan to run a server using the System V init
services as provided by the cyphesis rpm then most of this configuration
is handled for you automatically. If this is the case please see the
Section called Setup as a System Service for more information.

The first step is to setup the database access. The database is required
to store account and rule information. If full server persistence is
enabled, the database is also used to store the entire world state. A
postgresql server must be running on the system where you plan to run
cyphesis, and the user who will run cyphesis must have access to the
database. If you do not have root access on the system you will need to
contact the system administrator to request a database account. By default
cyphesis assumes that access to a PostgreSQL RDBMS running on the same
machine from a user account with the same name as the database account
does not require a password. If this is not the case you can either
configure the PostgreSQL RDBMS to work this way, or specify a password in
the config file.

Once database access has been granted you must run cyloadrules to load the
default rulesets into the database. For more information on how
cyloadrules works, including advanced usage see the Section called
Usage and configuration.

The server is now ready to run. For for more information on how to start
the server see the Section called Usage and configuration.

### Setup as a System Service

Running cyphesis as a service is the simplest way to get the server up and
running. If you are using rpm packages, the cyphesis rpm handles creating
a user account so that cyphesis does not run as the superuser. In order to
run the server correctly, the cyphesis service must be started, followed
by the cyclient service. This can be handled by configuring the system to
start these services at boot time, or by running the init scripts manually
as root as follows:

```
# /etc/init.d/cyphesis start
# /etc/init.d/cyclient start
```

The postgresql service is required and must be started before cyphesis.
The first time the cyphesis service is run, the init script will ensure
that cyphesis has access to the database, and will preload the database
with the neccessary data automatically.

If you are not using the packaged version of cyphesis, but wish to run it
as a system service, the init scripts are included in the top directory of
the source package and are called cyphesis.init and cyclient.init. Both of
these files should be installed in the init script directory on your
system, usually /etc/rc.d/init.d/. The procedure for enabling system
services varies from system to system. One command used for controlling
services is the chkconfig command, found on most Linux systems, and some
Unix variants. Once installed the scripts can be activated as follows:

```
# chkconfig --add cyphesis
# chkconfig --add cyclient
```

The services are then enabled as follows:

```
# chkconfig cyphesis on
# chkconfig cyclient on
```

For further details please see the chkconfig documentation. By default the
cyphesis init scripts attempt to run the server and client as a user
called cyphesis. An account with this username will need to be created
before the service will work. The file called cyphesis.sysconfig can
optionally be installed as /etc/sysconfig/cyphesis and edited to control
the username used to run the cyphesis server and client processes.

When cyphesis has been run as a system service, any error message or other
information are sent to the syslog. On most Linux systems this means that
you can see these message by looking at /var/log/messages. Please see the
syslog documentation for information about how to control these log
messages.


## Running a basic server

Before you run the server for the first time, run the cyloadrules to
prepare the database tables. The command should print out some message
indicating the number of rules loaded. You will not need to run this
command again unless you upgrade to a newer version of the rules or of
cyphesis.

Start the server with the cyphesis command. It will output some startup
messages and then run in the foreground. If you want to run the server in
the background, start the server with the option --cyphesis:daemon=true .

Each time the server is run it needs to be populated with game data before
it does anything useful. If you are running the server using the System V
init service then this is handled for you by the cyclient service. If you
are running the server manually you will need to run cyclient yourself. In
a separate terminal run the cyclient command, which will populate the
server, outputting messages as it does this. Once it has completed
cyclient will exit, and the server will be ready. The server will
automatically register its presence with the metaserver so you will not
need to advertise it.

If you everything has worked so far, and you are not planning to do any
server or world development at this time then you do not need to read any
of the rest of these instructions.

## Usage and configuration

The main server binary is called cyphesis. Its command line arguments and
configuration are managed by varconf, which means options can be set in
configuration files and on the command line. The main configuration file
is called cyphesis.vconf, and server settings are stored in the [cyphesis]
section. The file can be found in the cyphesis source directory, and is
installed into the sysconf directory, which is by default /etc. Settings
in this configuration file can be overridden in on the command line, and
once overridden they will be stored permanently in .cyphesis.vconf in the
users home directory. In order to drop back to the default settings,
remove this file. Settings can be incrementally overridden in
~/.cyphesis.vconf non-interactively by passing them as command line
options to cyconf. cyconf will store any settings it is given in
~/.cyphesis.vconf and then exited. If you are planning to have multiple
servers run on the same system at the same or different times, the easiest
way to handle the differences in configuration would be to use the
~/.cyphesis.vconf file, and avoid modifying the master configuration file.

As an example, the ruleset to be used is set in cyphesis.vconf as follows:
```
   [cyphesis]
   ruleset="deeds"
```
This setting can be overridden by invoking cyphesis with the following
option:
```
   --cyphesis:ruleset=werewolf
```
For more details of varconf usage see the Varconf documentation. For full
details on configuraton options for cyphesis, see the cyphesis(1) man
page.

The ruleset specified indicates the entity types available, the set of
scripts that will be used for these entities, and the initialisation
script used to populate the server.

If an empty server is started, it will automatically be populated if the 
[Worldforge Worlds](https://github.com/worldforge/worlds) definitions have 
been installed.

The default ruleset for this version is called Deeds.

Before you start the server for the first time, you will need to load some
data into the server's database tables. You will first have to load
ruleset data into the database. If this is the first time you have run
cyphesis, you will need to set it up so cyphesis has access. In order to
use databases, cyphesis needs to know the name of an account it can use,
and the name of a database where it can create its tables. By default it
uses the current user name to connect to PostgreSQL, and the name cyphesis
for the database. It has been assumed that PostgreSQL has been set up as
it is on most systems to accept a local connection from a user with the
same name as the database account name without a password. If you want to
go through the setup of the database manually, or for some reason
cyphesis-setup does not work, you will need to create a database account
with the right name, and a database belonging to that account called
cyphesis, or whatever name you choose to call it. For information on how
to do this, please see the PostgreSQL documentation provided with the
version you have installed.

Once cyphesis has access to the database, run cypasswd with no arguments
to set the admin password to something unique.

A ruleset will need to be loaded into the database before you can do
anything useful with the server. Each ruleset optionally depends on
another ruleset, so in addition to the ruleset you are using you will need
to load the rulesets on which it depends. A ruleset is distributed with
cyphesis as an atlas xml file. The default is mason.xml, which depends on
acorn.xml, which in turn depends on basic.xml. These three rulesets can be
loaded into the database using the cyloadrules command with no arguments
as follows:
```
   $ cyloadrules
   Reading rules from mason
   49 classes stored in rule database.
   Reading rules from basic
   29 classes stored in rule database.
   $
```
This automatically loads the rulesets in order into the database, first
ensuring that the rules table is empty.

cyloadrules can also be used to load individual rulesets into the database
as follows:
```
   $ cyloadrules mason.xml
   49 classes stored in rule database.
   $ cyloadrules basic.xml
   29 classes stored in rule database.
   $
```
You will only need to do this if you are developing new rulesets, or
customising existing ones.

The database store is persistent. If new a ruleset is provided, it will be
necessary to clear the database tables before loading them with new data.

## Copyright notices

The server code in C++ is distributed under the GNU General Public
License. See the file COPYING for details. The script files included with
this distribution are also distributed under the GNU General Public
License. Note that this copyright does not cover user scripts that use
server services but do not use code from the scripts provided. Using such
scripts is considered ordinary use of the server, and does not fall under
the heading of derived work.

## How to help

If you're interested in helping out with development you should check out these resources:

* [The main Worldforge site](http://worldforge.org/ "The main Worldforge site")
* [Bugs and feature planning on Launchpad](https://launchpad.net/cyphesis "Cyphesis Launchpad entry")
* [Gitter conversation](https://gitter.im/Worldforge/Lobby "Gitter conversation")
* [Mailing lists](http://mail.worldforge.org/lists/listinfo/ "Mailing lists")
