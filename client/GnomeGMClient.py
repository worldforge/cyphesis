#!/usr/bin/env python
#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2000 Aloril (See the file COPYING for details).
from GraphicalClient import *

from gtk import *
from gnome.ui import *
import GdkImlib

class ImageWrapper(GdkImlib.Image):
    def __init__(self, name):
        GdkImlib.Image.__init__(self, name)
        self.render()
    def GetWidth(self):
        return self.rgb_width
    def GetHeight(self):
        return self.rgb_height

background_map = BackgroundMap(ImageWrapper)

def deb(): import pdb; pdb.pm()

#not used currently:
def object_event(p, event, obj):
    #print "Object:", obj, obj.obj.id, "Event:", event, "P:", p
    global x,y
    if event.type == GDK.BUTTON_PRESS:
        if event.button == 1:
            # Remember starting position.
            x = event.x
            y = event.y
            p.raise_to_top()
            return TRUE
    elif event.type == GDK.MOTION_NOTIFY:
        if event.state & GDK.BUTTON1_MASK:
            # Get the new position and move by the difference
            new_x = event.x
            new_y = event.y

            p.move(new_x - x, new_y - y)

            x = new_x
            y = new_y

            return TRUE
    elif event.type == GDK.BUTTON_RELEASE:
        print "Released:",p,event.x,event.y

class SpriteInfo:
    text_border=2

    def __init__(self, canvas, obj):
        self.canvas = canvas
        self.obj = obj
        self.group = canvas.root().add('group')
        self.group.connect("event", object_event, self)
        self.coords = (0.0, 0.0)
        self.img = None
        self.text = ""
        self.text_obj = None
        self.text_frame = None
        self.update()

    def __cmp__(self, other):
        return media.cmp_media(self.obj, other.obj)

    def update(self):
        self.xyz = xyz = self.obj.get_xyz()
        x,y = background_map.scx(xyz[0]), background_map.scy(xyz[1])
        text = [self.obj.id]
        if hasattr(self.obj,"goal"):
            text.append(self.obj.goal)
        fname = "media"+os.sep+media.lookup_media_by_object(self.obj)
        img = self.canvas.get_image(fname)
        if (x,y)!=self.coords:
            #print "MOVING!", self.coords, "->", x,y, self.obj.id
            dx = x-self.coords[0]
            dy = y-self.coords[1]
            #print "Move d:", dx, dy
            self.coords = x,y
            self.group.move(dx, dy)
            self.group.raise_to_top()
        self.xyz = xyz
        if self.img!=img:
            if self.img:
                #print "REMOVING IMAGE!"
                self.img.destroy()
            #print "CREATING IMAGE!", fname
            self.img = self.group.add(
            #self.img = self.canvas.root().add(
                'image',
                x = 0, y = 0,
                width = img.GetWidth(), height = img.GetHeight(),
                image = img)
            #self.img.connect("event", object_event, self)

        if self.text != text:
            if self.text_obj:
                self.text_obj.destroy()
            if self.text_frame:
                self.text_frame.destroy()
            self.text = text
            self.text_obj = self.group.add(
                'text', 
                text = string.join(self.text,"\n"),
                x=0, y=img.GetHeight()/2,
                font='-adobe-helvetica-bold-r-normal--*-80-*-*-*-*-iso8859-1',
                anchor=ANCHOR_NORTH_WEST,
                fill_color='black')
            bounds = self.text_obj.get_bounds()
            self.text_frame = self.group.add(
                'rect',
                x1=bounds[0]-2, y1=bounds[1]-1,
                x2=bounds[2], y2=bounds[3],
                fill_color='white',
                outline_color='black')
            self.text_obj.raise_to_top()

    def destroy(self):
        self.group.destroy()


class WorldCanvas(GnomeCanvas):
    def __init__(self):
        GnomeCanvas.__init__(self)
        self.parent = self.root()
        
        self.maxWidth  = background_map.GetWidth()
        self.maxHeight = background_map.GetHeight()

        #print "CS:",self.maxWidth, self.maxHeight
        self.set_usize(self.maxWidth, self.maxHeight)
        self.show()
    
        self.background_object = self.root().add(
            'image',
            x = self.maxWidth/2, y = self.maxHeight/2,
            width = self.maxWidth, height = self.maxHeight,
            image = background_map.map_image)
        #self.background_object.connect("event", object_event, self)

        self.set_scroll_region(0, 0, self.maxWidth, self.maxHeight)
        #print self.background_object.get_bounds()

##         for i in range(-220,330,20):
##             text = self.root().add(
##                 'text', 
##                 text="%d" % i,
##                 x=i, y=i,
##                 font='-adobe-helvetica-bold-r-normal--24-240-75-75-p-*-iso8859-1',
##                 anchor=ANCHOR_CENTER,
##                 fill_color='white')
##             #text.connect("event", object_event)
        
        self.status_text = None

        self.sprites={}
        self.typeImages = {}

    def set_gm(self, gm):
        self.gm=gm
        idle_add(self.idle)

    def idle(self):
        self.gm.run(loop=0)
        idle_add(self.idle)

    def add_object(self, obj):
        if obj.type and obj.type[0] in known_types:
            self.sprites[obj.id]=SpriteInfo(self,obj)

    def update_object(self, obj):
        #type was changed (got more info than mere id)
        if obj.type and obj.type[0] in known_types:
            if self.sprites.has_key(obj.id):
                self.sprites[obj.id].update()
            else:
                print "Type changed and thus it became visible:",obj
                self.sprites[obj.id]=SpriteInfo(self,obj)

    def delete_object(self, obj):
        sprite = self.sprites.get(obj.id)
        if sprite:
            sprite.destroy()
            del self.sprites[obj.id]

    def GetWidth(self):
        return self.maxWidth

    def GetHeight(self):
        return self.maxHeight

    def get_image(self, fname):
        img = self.typeImages.get(fname)
        if not img:
            test_file_existence(fname)
            img = ImageWrapper(fname)
            self.typeImages[fname]=img
        return img

    def SetStatusText(self, text):
        if self.status_text:
            self.status_text.destroy()
        self.status_text = self.root().add(
            'text', 
            text=text,
            x=10, y=self.GetHeight()-10,
            font='-adobe-helvetica-bold-r-normal--*-100-*-*-p-*-iso8859-1',
            anchor=ANCHOR_WEST,
            fill_color='white')


if __name__=="__main__":
    win = GtkWindow()
    win.connect('destroy', mainquit)
    win.set_title('GM Client')
    canvas = WorldCanvas()
    win.add(canvas)

    #status.set_justify(JUSTIFY_LEFT)
    #status.set_alignment(0.0, 0.5)

    win.show()


    gm=GraphicalClient(sys.argv,"gm.log", canvas)
    gm.load_default()

    canvas.set_gm(gm)

    mainloop()

