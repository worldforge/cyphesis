# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 1999 Aloril (See the file COPYING for details).

semnet = {}


class semnod:
    def __init__(self, id):
        self.id = id
        self.semlinks = []

    def add_link(self, link):
        self.semlinks.append(link)

    def __cmp__(self, other):
        if self.id == other.id: return 0
        return 1

    def __str__(self):
        s = '<semnod id="' + self.id + '">'
        for l in self.semlinks:
            s = s + str(l)
        return s + "</semnod>"

    def __repr__(self):
        return 'semnod(' + repr(self.id) + ')'


def find_semnod(o, id):
    if not id: return None
    if id not in o: o[id] = semnod(id)
    return o[id]


class semlink:
    # fr: short of from (from is reserved word in Python)
    def __init__(self, fr, type, to, mass, o):
        self.o = o
        self.fr = self.link_semnod(fr)
        self.to = self.link_semnod(to)
        self.type = type
        self.mass = mass

    def link_semnod(self, id):
        n = find_semnod(self.o, id)
        n.add_link(self)
        return n

    def __str__(self):
        return '<semlink from="' + self.fr.id + '" to="' + self.to.id + \
               '" mass=' + repr(self.mass) + '>' + self.type + '</semlink>'


class atom:
    def __init__(self, word, id, lexlink=(None, ""),
                 synlink=(None, ""), synlink_dir=""):
        self.word = word
        self.id = id
        self.lexlink = find_semnod(semnet, lexlink[0])
        self.synlink = synlink[0]
        self.lexlink_type = lexlink[1]
        self.synlink_type = synlink[1]
        self.right = self.left = self.down = self.up = None
        if synlink_dir: self.link(self.synlink, synlink_dir)

    def link(self, a2, dir):
        opp = {"up": "down",
               "down": "up",
               "left": "right",
               "right": "left"}
        setattr(self, dir, a2)
        setattr(a2, opp[dir], self)

    def match(self, t, res):
        # t=template
        # print "M:",`self`,`t`
        if t.word and t.word != self.word: return {}
        if t.id and t.id != self.id: return {}
        l = t.lexlink
        if l:
            if l.id[0] == '?':
                res[l.id[1:]] = self.lexlink
            else:
                if l != self.lexlink: return {}
        if t.lexlink_type and t.lexlink_type != self.lexlink_type: return {}
        if t.synlink_type and t.synlink_type != self.synlink_type: return {}
        if t.down:
            if not self.down: return {}
            if not self.down.match(t.down, res): return {}
        if t.right:
            if not self.right: return {}
            if not self.right.match(t.right, res): return {}
        return res

    def id_str(self):
        return self.word + repr(self.id)

    def __str__(self):
        scr = screen()
        scr.dsp_syn(0, 1, self)
        return str(scr)

    ##         lexlink_s='<lexlink href="'+self.lexlink.id+'">'+self.lexlink_type+\
    ##                   '</lexlink>\n'
    ##         synlink_dir_s=""
    ##         for d in ["left","up"]:
    ##             l=getattr(self,d)
    ##             if l: synlink_dir_s=' '+d+'="#'+l.id_str()+'"'
    ##         synlink_s='<synlink'+synlink_dir_s+'>'+self.synlink_type+'</synlink>\n'
    ##         return '<atom name="'+self.id_str()+'">\n'+lexlink_s+synlink_s+'</atom>'
    def __repr__(self):
        return repr([self.word, self.id, self.lexlink_type,
                     self.synlink_type])


def match_sentence(sentence, template):
    if not (template and sentence): return {}
    return sentence[0].match(template[0], {"ok": 1})


class screen:
    def __init__(self):
        self.s = {}
        self.xmax = 0
        self.ymax = 0

    def print_xy(self, x, y, txt):
        if not txt: return
        if y >= self.ymax: self.ymax = y + 1
        if x + len(txt) - 1 >= self.xmax: self.xmax = x + len(txt)
        for i in range(len(txt)):
            if x + i not in self.s: self.s[x + i] = {}
            self.s[x + i][y] = txt[i]

    def dsp_syn(self, x, y, a):
        self.print_xy(x, y, a.word)
        if a.down:
            self.print_xy(x, y + 1, "^")
            self.print_xy(x, y + 2, "|" + a.down.synlink_type)
            self.print_xy(x, y + 3, "|")
            self.dsp_syn(x, y + 4, a.down)
        if a.right:
            x = x + len(a.word) + 1
            self.print_xy(x, y, "<-----------")
            self.print_xy(x, y - 1, a.right.synlink_type)
            self.dsp_syn(x + 11 + 2, y, a.right)

    def dsp_simple_syn(self, x, y, a):
        self.print_xy(x, y, "O")
        if a.down:
            self.print_xy(x, y + 1, "|")
            self.print_xy(x, y + 2, "v")
            self.dsp_simple_syn(x, y + 3, a.down)
        if a.right:
            x = x + 1
            self.print_xy(x, y, "->")
            self.dsp_simple_syn(x + 3, y, a.right)

    def __str__(self):
        s = []
        for y in range(self.ymax):
            for x in range(self.xmax):
                try:
                    s.append(self.s[x][y])
                except KeyError:
                    s.append(' ')
            s.append('\n')
        return str.join(s, '')


def str_sentence(sentence):
    return str(sentence[0])
