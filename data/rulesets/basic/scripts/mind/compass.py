# This file may be redistributed and modified only under the terms of
# the GNU General Public License (See COPYING for details).
# Copyright (C) 2005 Alistair Riddoch

from physics import Vector3D

compass_points = [(Vector3D(1, 0, 0), "east"),
                  (Vector3D(0, 0, -1), "north"),
                  (Vector3D(0, 0, 1), "south"),
                  (Vector3D(-1, 0, 0), "west"),
                  (Vector3D(0.707, 0, -0.707), "north east"),
                  (Vector3D(-0.707, 0, -0.707), "north west"),
                  (Vector3D(0.707, 0, 0.707), "south east"),
                  (Vector3D(-0.707, 0, 0.707), "south west")]


def vector_to_compass(direction):
    dot = -1
    ret = "unknown"
    dir = direction.unit_vector()
    for point in compass_points:
        new_dot = dir.dot(point[0])
        if new_dot > dot:
            dot = new_dot
            ret = point[1]
    return ret
