import re
import sys

syntax_desc = re.compile("(\S+)\s+(\d+)\s*([^,]*)\s*,\s*(.*?)\s*$")


class atom:
    def __init__(self, word, sib, lexlink, synlink):
        self.word = word
        self.sib = int(sib)
        self.lexlink_type = lexlink
        self.synlink_type = synlink
        self.lexlink = self.synlink = None
        self.right = self.left = self.down = self.up = None
        self.id = 0

    def id_str(self):
        return self.word + "#" + repr(self.id)

    def str_link(self, lnk):
        l = getattr(self, lnk)
        if l: return lnk + "=" + l.word
        return ""

    def __str__(self):
        s = self.word + " " + repr(self.sib) + " " + self.lexlink_type + ", " + self.synlink_type
        for l in ["lexlink", "synlink", "right", "left", "up", "down"]:
            res = self.str_link(l)
            if res: s = s + ", " + res
        return s

    def __repr__(self):
        return repr([self.word, self.sib, self.lexlink_type,
                     self.synlink_type])


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


def build_syn_tree(arr):
    if not arr: return None
    a = arr[0]
    a.down = build_syn_tree(arr[1:a.sib])
    if a.down: a.down.up = a
    a.right = build_syn_tree(arr[a.sib:])
    if a.right: a.right.left = a
    return a


def list_syntax(syn_arr):
    for a in syn_arr:
        print(a.id_str(), "sib(" + repr(a.sib) + "),", \
              "lexlink(" + a.lexlink_type + "),")
        if a.up:
            l = "up to " + a.up.id_str()
        elif a.left:
            l = "left to " + a.left.id_str()
        else:
            l = ""
        print("\tsynlink(" + a.synlink_type + ")", l)


def display_syntax(syn_tree):
    scr = screen()
    scr.dsp_syn(0, 1, syn_tree)
    print(scr)


def display_simple_syntax(syn_tree):
    scr = screen()
    scr.dsp_simple_syn(0, 0, syn_tree)
    print(scr)


def get_tree(atom):
    if not atom: return None
    down = get_tree(atom.down)
    right = get_tree(atom.right)
    return (atom.word, atom.lexlink_type, atom.synlink_type, down, right)


def read_syntax_description(file_name):
    fp = open(file_name)
    line = fp.readline()
    arr = []
    id = 0
    while line:
        if line[0] != '#':
            res = syntax_desc.match(line)
            a = atom(res.group(1), res.group(2), res.group(3), res.group(4))
            id = id + 1
            a.id = id
            arr.append(a)
        line = fp.readline()
    fp.close()
    build_syn_tree(arr)
    return arr


def deb(): import pdb; pdb.pm()


def a(string):
    global syn_arr
    syn_arr = read_syntax_description(string)
    list_syntax(syn_arr)
    display_syntax(syn_arr[0])
    display_simple_syntax(syn_arr[0])


if __name__ == "__main__":
    if len(sys.argv) < 2:
        sys.argv.append("syntax_input.txt")
    a(sys.argv[1])
