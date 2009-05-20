// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id$

#include <Python.h>

#include "rulesets/Python_API.h"

#include <cassert>

int main()
{
    init_python_api();

    assert(PyRun_SimpleString("print 'hello'") == 0);
    assert(PyRun_SimpleString("import sys") == 0);
    assert(PyRun_SimpleString("sys.stdout.write('hello')") == 0);
    assert(PyRun_SimpleString("sys.stdout.write(1)") == -1);
    assert(PyRun_SimpleString("sys.stderr.write('hello')") == 0);
    assert(PyRun_SimpleString("sys.stderr.write(1)") == -1);

    assert(PyRun_SimpleString("from common import log") == 0);
    assert(PyRun_SimpleString("log.debug('foo')") == 0);
    assert(PyRun_SimpleString("log.thinking('foo')") == 0);

    assert(PyRun_SimpleString("import atlas") == 0);

    assert(PyRun_SimpleString("l=atlas.Location()") == 0);
    assert(PyRun_SimpleString("atlas.isLocation(l)") == 0);
    assert(PyRun_SimpleString("atlas.isLocation(1)") == 0);
    assert(PyRun_SimpleString("l1=atlas.Location()") == 0);
    assert(PyRun_SimpleString("l2=atlas.Location()") == 0);

    assert(PyRun_SimpleString("import physics") == 0);
    assert(PyRun_SimpleString("physics.distance_to()") == -1);
    assert(PyRun_SimpleString("physics.square_distance()") == -1);
    assert(PyRun_SimpleString("physics.square_horizontal_distance()") == -1);
    assert(PyRun_SimpleString("physics.distance_to(l1, l2)") == 0);
    assert(PyRun_SimpleString("physics.square_distance(l1, l2)") == 0);
    assert(PyRun_SimpleString("physics.square_horizontal_distance(l1, l2)") == 0);
    assert(PyRun_SimpleString("physics.distance_to('1', l2)") == -1);
    assert(PyRun_SimpleString("physics.square_distance('1', l2)") == -1);
    assert(PyRun_SimpleString("physics.square_horizontal_distance('1', l2)") == -1);

    assert(PyRun_SimpleString("atlas.Entity()") == 0);
    assert(PyRun_SimpleString("e=atlas.Entity('1')") == 0);
    assert(PyRun_SimpleString("e=atlas.Entity(1)") == -1);
    assert(PyRun_SimpleString("e=atlas.Entity('1', location='loc')") == -1);
    assert(PyRun_SimpleString("e=atlas.Entity('1', location=l)") == 0);
    assert(PyRun_SimpleString("e=atlas.Entity('1', pos=())") == 0);
    assert(PyRun_SimpleString("e=atlas.Entity('1', pos=[])") == 0);
    assert(PyRun_SimpleString("e=atlas.Entity('1', pos=(1,1.0,'1'))") == -1);
    assert(PyRun_SimpleString("e=atlas.Entity('1', pos=[1,1.0,'1'])") == -1);
    assert(PyRun_SimpleString("e=atlas.Entity('1', pos=1)") == -1);
    assert(PyRun_SimpleString("e=atlas.Entity('1', parent=1)") == -1);
    assert(PyRun_SimpleString("e=atlas.Entity('1', parent='0')") == 0);
    assert(PyRun_SimpleString("e=atlas.Entity('1', type=1)") == -1);
    assert(PyRun_SimpleString("e=atlas.Entity('1', type='pig')") == 0);
    assert(PyRun_SimpleString("e=atlas.Entity('1', other=1)") == 0);

    assert(PyRun_SimpleString("o=atlas.Operation()") == -1);
    assert(PyRun_SimpleString("o=atlas.Operation('get')") == 0);
    assert(PyRun_SimpleString("o=atlas.Operation('thought')") == 0);
    assert(PyRun_SimpleString("o=atlas.Operation('not valid')") == -1);
    assert(PyRun_SimpleString("o=atlas.Operation('get', to='1', from_='1')") == 0);
    assert(PyRun_SimpleString("o=atlas.Operation('get', to=e, from_=e)") == 0);
    assert(PyRun_SimpleString("o=atlas.Operation('get', atlas.Entity(), to='1', from_='1')") == 0);
    assert(PyRun_SimpleString("o=atlas.Operation('get', atlas.Operation('set'), to='1', from_='1')") == 0);
    assert(PyRun_SimpleString("o=atlas.Operation('get', atlas.Location(), to='1', from_='1')") == -1);
    assert(PyRun_SimpleString("o=atlas.Operation('get', atlas.Entity(), atlas.Entity(), atlas.Entity(), to='1', from_='1')") == 0);

    assert(PyRun_SimpleString("atlas.Message(atlas.Operation('get'))") == 0);

    assert(PyRun_SimpleString("from physics import BBox") == 0);
    assert(PyRun_SimpleString("b=BBox()") == 0);
    assert(PyRun_SimpleString("b=BBox([1])") == -1);
    assert(PyRun_SimpleString("b=BBox([1,1,1])") == 0);
    assert(PyRun_SimpleString("b=BBox([1.0,1.0,1.0])") == 0);
    assert(PyRun_SimpleString("b=BBox(['1','1','1'])") == -1);
    assert(PyRun_SimpleString("b=BBox(1)") == -1);
    assert(PyRun_SimpleString("b=BBox(1,1)") == -1);
    assert(PyRun_SimpleString("b=BBox(1,1,1)") == 0);
    assert(PyRun_SimpleString("b=BBox(1.0,1.0,1.0)") == 0);
    assert(PyRun_SimpleString("b=BBox(1.0,1.0,1.0,1.0,1.0,1.0)") == 0);
    assert(PyRun_SimpleString("b=BBox('1','1','1')") == -1);

    assert(PyRun_SimpleString("import Quaternion") == 0);
    assert(PyRun_SimpleString("q=Quaternion.Quaternion()") == 0);
    assert(PyRun_SimpleString("q=Quaternion.Quaternion([1])") == -1);
    assert(PyRun_SimpleString("q=Quaternion.Quaternion([0,0,0,1])") == 0);
    assert(PyRun_SimpleString("q=Quaternion.Quaternion([0.0,0.0,0.0,1.0])") == 0);
    assert(PyRun_SimpleString("q=Quaternion.Quaternion(['0.0',0.0,0.0,1.0])") == -1);
    assert(PyRun_SimpleString("q=Quaternion.Quaternion(1,0)") == -1);
    assert(PyRun_SimpleString("from physics import Vector3D") == 0);
    assert(PyRun_SimpleString("q=Quaternion.Quaternion(Vector3D(1,0,0),0)") == -1);
    assert(PyRun_SimpleString("q=Quaternion.Quaternion(Vector3D(1,0,0),0.0)") == 0);
    assert(PyRun_SimpleString("q=Quaternion.Quaternion(Vector3D(1,0,0),Vector3D(0,1,0))") == 0);
    assert(PyRun_SimpleString("q=Quaternion.Quaternion(1,0,0)") == -1);
    assert(PyRun_SimpleString("q=Quaternion.Quaternion(0,0,0,1)") == 0);
    assert(PyRun_SimpleString("q=Quaternion.Quaternion(0.0,0.0,0.0,1.0)") == 0);
    assert(PyRun_SimpleString("q=Quaternion.Quaternion('0.0',0.0,0.0,1.0)") == -1);

    shutdown_python_api();
    return 0;
}
