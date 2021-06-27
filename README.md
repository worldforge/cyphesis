# Cyphesis, the Worldforge server

[![Join us on Gitter!](https://badges.gitter.im/Worldforge.svg)](https://gitter.im/Worldforge/Lobby)
[![Build all](https://github.com/worldforge/cyphesis/actions/workflows/cmake.yml/badge.svg)](https://github.com/worldforge/cyphesis/actions/workflows/cmake.yml)

[![Get it from the Snap Store](https://snapcraft.io/static/images/badges/en/snap-store-black.svg)](https://snapcraft.io/cyphesis)

Cyphesis is the server for the [WorldForge](http://worldforge.org/ "The main Worldforge site") system.

It provides a complete solution for running an MMORPG server. Amongst its features are

* Fully scriptable through Python
* Live reload of both rules and world entities; edit your world without having to shut down or reload
* Complete 3d physics simulation
* Complex AI system, using Behavioral Trees and Python scripts
* Out-of-process AI, allowing for distributed AI clients
* Persistence through either SQLite or PostgreSQL
* Powerful built in rules for visibility and containment of entities
* Emergent gameplay through multiple simple systems interacting
* Quick and powerful procedural terrain generation  

## Installation

The simplest way to install all required dependencies is by using [Conan](https://www.conan.io).

```bash
conan remote add worldforge https://artifactory.ogenvik.org/artifactory/api/conan/conan
mkdir build && cd build
conan install .. --build missing
cmake ..
make -j all assets-download install
```

Alternatively you can use the [Hammer](http://wiki.worldforge.org/wiki/Hammer_Script "The Hammer script") tool.
This is script provided by the Worldforge project which will download and install all of the required libraries and
components used by Worldforge.

Otherwise the server can most easily be built through the following commands.
```
mkdir build_`arch` && cd build_`arch`
cmake ..
make
make assets-download
make install
```

Note that you also probably want to install the defaults worlds from 
[Worldforge Worlds](https://github.com/worldforge/worlds) (this is done automatically by the
Hammer build tool).

### Tests

The test suite can be built and run using the ```check``` target. For example:

```
make check
```

### Documentation

Documentation describing how the system works can be found [here](docs/dox/index.md).

There's also a collection of design documents found in the ["docs/design" directory](docs/design)

### API documentation

If Doxygen is available API documentation can be generated using the ```dox``` target. For example:

```
make dox
```

## Dependencies

Cyphesis is built using CMake. 
It requires Python and SQLite 3 which are included with most Linux
distributions, and Atlas-C++, varconf, Mercator and wfmath 
which are provided by the WorldForge project. GNU readline is required by
some of the included tools.
Boost is used as well, mainly for the ASIO library which drives networking.

If a PostgreSQL development package is detected the optional PostgreSQL
database backend will be built. This has to be enabled through a config option;
by default data is persisted through SQLite.

If built from source the software and data must be installed using "make
install" before it will be ready. Go to the Section called Setup for
information on the setup steps required after installation.


## Running a basic server

Start the server with the ```cyphesis``` command. It will output some startup
messages and then run in the foreground. If you want to run the server in
the background, start the server with the option --cyphesis:daemon=true .

If an empty server is started, it will automatically be populated if the 
[Worldforge Worlds](https://github.com/worldforge/worlds) definitions have 
been installed.  

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

## User provided Python scripts

When both the "cyphesis" server and the "cyaiclient" AI process starts they look
for extra python scripts to run in the "~/.local/share/cyphesis/cyaiclient.d"
and "~/.local/share/cyphesis/cyphesis.d" directories respectively.

This allows a developer to for example set up Python remote debugging.
One example would be to use the remote Python debugger included in PyCharm
Professional. By creating a file named "10-debug.py" containing this code
a connection will be made at startup with the debugger on port 9999.
```python
import sys
sys.path.append('path_to_pycharm-debug-py3k.egg')
import pydevd
pydevd.settrace('localhost', port=9999, stdoutToServer=False, stderrToServer=False, suspend=False)
```

## Using standard malloc memory allocator in Python

During development it can sometimes be good to use the standard "malloc" memory allocator rather than 
the Python specific allocator. For example if you want to profile memory usage.
This can be enabled by setting the environment variable "PYTHONMALLOC" to something (doesn't matter what).
Upon startup Cyphesis will then use malloc, and write a line about this to the log.

## Performance measuring through Remotery

Performance tracking through [Remotery](https://github.com/Celtoys/Remotery) is built in, 
but not enabled by default. You need to set the option "--cyphesis:remotery=true" to enable it.

After that's done you can open the file found at [external/Remotery/vis/index.html]() to see
real time measurements.

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
