#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 kfort (See the file COPYING for details).

import atlas
import atlasXML
from common.lookup import get_thing_class
import string,random
from server.Admin import Admin

admin_file="/tmp/admin.xml"

def save_meta(server, filename):
    fd = open(filename, 'w')
    fd.write("<atlas>\n")
    msg=atlas.Message()
    for obj in server.objects.values():
        msg.append(obj.as_entity())
    fd.write(str(msg)+"\n")
    fd.write("</atlas>\n")
    fd.close()

def load_meta(server, filename):
    fd = open(filename, 'r')
    parse_objects = atlasXML.get_parser()
    entities=parse_objects(fd.read())
    fd.close()
    for ent in entities:
        type = string.capitalize(ent.type[0])
        type_class = get_thing_class(type)
        obj=server.add_object(type_class,ent)

def save_world(world, filename):
    fd = open(filename, 'w')
    fd.write("<atlas>\n")
    for key in world.objects.keys():
        if key not in ('illegal', 'world_0'):
            fd.write(str(world.objects[key].as_entity()) + "\n")
    fd.write("</atlas>\n")
    fd.close()

def load_world(world, filename):
    fd = open(filename, 'r')
    parser = atlasXML.get_parser()
    parser.parse_stream(fd.read())
    fd.close()
    for node in parser.tree.child:
        ent = parser.parse_ent(node)
        type = string.capitalize(ent.type[0])
        type_class = get_thing_class(type)
        obj = atlas.object_from_entity(type_class, ent)
        world.server.id_dict[obj.id] = world.objects[obj.id] = obj

def load_admin_account():
    try:
        fd = open(admin_file)
    except IOError:
        password=""
        while len(password)<8:
            password=password+random.choice(string.letters+string.digits)
        obj=Admin(id="admin",password=password)
        fd = open(admin_file, "w")
        fd.write("<atlas>\n<obj>\n")
        fd.write(str(obj.as_entity()))
        fd.write("</obj>\n</atlas>\n")
        fd.close()
        return obj
    parse_objects = atlasXML.get_parser()
    entities=parse_objects(fd.read())
    fd.close()
    for ent in entities:
        if ent.type[0]=="admin":
            return atlas.object_from_entity(Admin, ent)
