#!/usr/bin/env python
#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2000 Aloril (See the file COPYING for details).
from GraphicalClient import *

from wxPython.wx import *

wxImage_AddHandler(wxJPEGHandler())
wxImage_AddHandler(wxPNGHandler())

def createImg(name):
    return wxImage(name, wxBITMAP_TYPE_JPEG).ConvertToBitmap()

background_map = BackgroundMap(createImg)

class IdleTimer(wxTimer):
    def __init__(self, func):
        wxTimer.__init__(self)
        self.callback = func
        self.Start(100)
        self.mathing_lines=[]
    def Notify(self):
        self.Stop()
        self.callback()
        self.Start(100)

def imageSize(image):
    return wxSize(image.GetWidth(), image.GetHeight())

class SpriteInfo:
    text_border=2
    def __init__(self, canvas, obj):
        self.canvas = canvas
        self.obj = obj
    def __cmp__(self, other):
        return media.cmp_media(self.obj, other.obj)
    def erase_draw_text(self, dc, str, x, y):
        w,h = dc.GetTextExtent(str)
        r = wxRect(x,y,w,h)
        #self.canvas.Refresh(0, r)
        b=self.text_border
        dc.DrawRectangle(x-b,y-b,w+b*2,h+b*2)
        dc.DrawText(str, x, y)
        return r
    def draw(self, dc):
        #clean old picture ...
        self.xyz = xyz = self.obj.get_xyz()
        x,y = background_map.scx(xyz[0]), background_map.scy(xyz[1])
        text = [self.obj.id]
        if hasattr(self.obj,"goal"):
            text.append(self.obj.goal)
        fname = "media"+os.sep+media.lookup_media_by_object(self.obj)
        img = self.canvas.get_image(fname)
        if hasattr(self,"coords") and \
           (self.xyz!=xyz or self.coords!=(x,y) or \
            self.text!=text or self.img != img):
            self.clear(dc)
        self.xyz = xyz
        self.coords = x,y
        self.text = text
        self.img = img

        w, h = img.GetWidth(), img.GetHeight()
        dc.DrawBitmap(img, x-w/2, y-h/2, 1)
        r = self.erase_draw_text(dc, self.text[0] ,x,y+h/2)
        if len(self.text)>1:
            r = self.erase_draw_text(dc, self.text[1], x,
                                     y+h/2+r.height+self.text_border*2)
    def clear(self, dc):
        if not hasattr(self,"coords"): return #no previous draw
        x,y = self.coords
        w,h = self.img.GetWidth(), self.img.GetHeight()
        r = wxRect(x-w/2, y-h/2, w, h)

        w2,h2 = dc.GetTextExtent(self.text[0])
        if len(self.text)>1:
            w3, h3 = dc.GetTextExtent(self.text[1])
            h2 = h2+h3
            w2 = max(w2,w3)
        w = max(w, w2) + self.text_border*4
        h = h + h2 + self.text_border*4
        self.canvas.Refresh(0, wxRect(x,y,w,h))
    def destroy(self):
        dc = wxPaintDC(self.canvas)
        dc.BeginDrawing()
        self.clear(dc)
        dc.EndDrawing()

class WorldCanvas(wxScrolledWindow):
    def __init__(self, parent):
        self.parent = parent
        wxScrolledWindow.__init__(self, parent, -1, wxPoint(0, 0), wxSize(500,500), wxSUNKEN_BORDER)
        
        self.lines = []
        self.maxWidth  = background_map.GetWidth()
        self.maxHeight = background_map.GetHeight()

        self.SetBackgroundColour(wxNamedColor("WHITE"))
        self.Connect(-1, -1, wxEVT_LEFT_DOWN, self.OnLeftButtonEvent)
        self.Connect(-1, -1, wxEVT_LEFT_UP,   self.OnLeftButtonEvent)
        self.Connect(-1, -1, wxEVT_MOTION,    self.OnLeftButtonEvent)

        self.background_map=wxStaticBitmap(self, -1, background_map, wxPoint(0, 0),
                                           imageSize(background_map))

        self.SetScrollbars(20, 20, self.maxWidth/20, self.maxHeight/20)

        self.sprites={}
        self.deleted_sprites=[]
        
        self.typeImages = {}

    def set_gm(self, gm):
        self.gm=gm
        self.timer = IdleTimer(self.idle)
        self.timer.Start(1000)
        self.lastTime=time()

    def idle(self):
        self.gm.run(loop=0)
        dc = wxPaintDC(self)
        self.PrepareDC(dc)
        self.DoDrawing(dc)

    def add_object(self, obj):
        if obj.type and obj.type[0] in known_types:
            self.sprites[obj.id]=SpriteInfo(self,obj)

    def update_object(self, obj):
        #type was changed (got more info than mere id)
        if obj.type and obj.type[0] in known_types and \
           not self.sprites.has_key(obj.id):
            print "Type changed and thus it became visible:",obj
            self.sprites[obj.id]=SpriteInfo(self,obj)

    def delete_object(self, obj):
        sprite = self.sprites.get(obj.id)
        if sprite:
            self.deleted_sprites.append(sprite)
            del self.sprites[obj.id]

    def getWidth(self):
        return self.maxWidth

    def getHeight(self):
        return self.maxHeight

    def DoDrawing(self, dc):
        dc.BeginDrawing()
        for sprite in self.deleted_sprites:
            sprite.clear(dc)
        self.deleted_sprites=[]
        sprite_list = self.sprites.values()
        sprite_list.sort()
        for sprite in sprite_list:
            sprite.draw(dc)
        dc.EndDrawing()
##         for sprite in sprite_list:
##             if hasattr(sprite,"text") and \
##                len(sprite.text)>1 and string.find(sprite.text[1],"buy_thing")>=0:
##                 self.gm.character.lock=1

    def get_image(self, fname):
        img = self.typeImages.get(fname)
        if not img:
            test_file_existence(fname)
            img = wxImage(fname, wxBITMAP_TYPE_PNG).ConvertToBitmap()
            self.typeImages[fname]=img
        return img

    def SetXY(self, event):
        self.x, self.y = self.ConvertEventCoords(event)

    def ConvertEventCoords(self, event):
        xView, yView = self.ViewStart()
        xDelta, yDelta = self.GetScrollPixelsPerUnit()
        return (event.GetX() + (xView * xDelta),
                event.GetY() + (yView * yDelta))

    def OnLeftButtonEvent(self, event):
        if event.LeftDown():
            self.SetXY(event)
            self.curLine = []

        elif event.Dragging():
            dc = wxClientDC(self)
            self.PrepareDC(dc)
            dc.BeginDrawing()
            dc.SetPen(wxPen(wxNamedColour('MEDIUM FOREST GREEN'), 4))
            coords = (self.x, self.y) + self.ConvertEventCoords(event)
            self.curLine.append(coords)
            apply(dc.DrawLine, coords)
            self.SetXY(event)
            dc.EndDrawing()

        elif event.LeftUp():
            self.lines.append(self.curLine)
            self.curLine = []

    def SetStatusText(self, text):
        self.parent.SetStatusText(text)


ID_ABOUT = 101
ID_EXIT  = 102

class GMFrame(wxFrame):
    def __init__(self, parent, ID, title):
        wxFrame.__init__(self, parent, ID, title,
                         wxDefaultPosition,
                         wxSize(background_map.GetWidth()+11,
                                background_map.GetHeight()+59))
        self.CreateStatusBar()
        self.SetStatusText("Loading...")
        
        menu = wxMenu()
        menu.Append(ID_ABOUT, "&About",
                    "More information about this program")
        menu.AppendSeparator()
        menu.Append(ID_EXIT, "E&xit", "Terminate the program")
        
        menuBar = wxMenuBar()
        menuBar.Append(menu, "&File");
        
        self.SetMenuBar(menuBar)
        self.canvas=WorldCanvas(self)
        


class GMApp(wxApp):
    def OnInit(self):
        self.frame = GMFrame(NULL, -1, "GM Client")
        self.frame.Show(true)
        self.SetTopWindow(self.frame)
        return true
    def get_canvas(self):
        return self.frame.canvas
    def set_gm(self, gm):
        self.frame.canvas.set_gm(gm)

if __name__=="__main__":
    app = GMApp(0)
    gm=GraphicalClient(sys.argv,"gm.log",app.get_canvas())
    gm.load_default()
    app.set_gm(gm)
    app.MainLoop()
