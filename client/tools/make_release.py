#!/usr/bin/env python
import os, sys
import string, re
from v0_0_9 import *

prog_dir = sys.argv[0]
#print "?", prog_dir
#try first sys.argv path part:
if prog_dir: 
    prog_dir = os.path.split(prog_dir)[0]
    #print "->", prog_dir
#not enough info or not given, try current working directory instead:
if len(prog_dir)<=1:
    prog_dir = os.getcwd()
    #print "=>", prog_dir
#remove "tools" part
base_dir = prog_dir[:-5]

def analyse_changelog(change_log = base_dir+"ChangeLog"):
    changes_since_last_release = []
    for line in open(change_log).readlines():
        if string.find(line, changelog_delimiter)>=0:
            return string.join(changes_since_last_release,"")
        changes_since_last_release.append(line)
    raise ValueError, "didn't find change_delimiter (=%s)" % changelog_delimiter

def write_all(fds, str):
    for fd in fds:
        fd.write(str)

class Output:
    def __init__(self, name):
        self.name = name
        self.fp = open(name, "w")
    def __getattr__(self, name):
        return getattr(self.fp, name)

class HtmlOutput(Output):
    def write(self, str):
        lines = string.split(str,"\n")
        last_was_list = 0
        for i in range(len(lines)):
            lines[i] = string.replace(lines[i], "&", "&amp;")
            lines[i] = string.replace(lines[i], "<", "&lt;")
            lines[i] = string.replace(lines[i], ">", "&gt;")
            if not lines[i]:
                lines[i] = "<p>"
            elif "-"*len(lines[i]) == lines[i]:
                lines[i-1] = "<b>" + lines[i-1]
                lines[i] = "</b>"
            if "- " == lines[i][:2]:
                if not last_was_list:
                    last_was_list = 1
                    lines[i] = "<ul><li>" + lines[i]
                else:
                    lines[i] = "<li>" + lines[i]
            else:
                if last_was_list and lines[i][0]!=" ":
                    last_was_list = 0
                    lines[i] = "</ul>" + lines[i]
            url_match = re.match(r"(.*)((ftp)|(http)://\S*)(.*)", lines[i])
            if url_match:
                lines[i] = '%s<a href="%s">%s</a>%s' % (
                    url_match.group(1), url_match.group(2),
                    url_match.group(2), url_match.group(5))
            em_match = re.match(r"(.*)\*([\w ]+)\*(.*)", lines[i])
            if em_match:
                lines[i] = '%s<em>%s</em>%s' % em_match.groups(1)
            if lines[i][-1]==':':
                if last_was_list:
                    lines[i] = lines[i] + "<br>"
                else:
                    lines[i] = "<b>%s</b>" % lines[i]
            
        for line in lines:
            self.fp.write(line+"\n")

if __name__=="__main__":
    print "Making release based on", base_dir
    change_log = analyse_changelog()

    short_announce = Output("announce.mail")
    detailed_announce = Output("detailed_announce.mail")
    wiki_announce = HtmlOutput("wiki_announce.mail")

    short_announce.write("To: announce@worldforge.org\n")
    detailed_announce.write("To: server@worldforge.org, cyphesis-snapshot@lists.1ststep.net, cyphesis-announce@lists.1ststep.net, lycadican@worldforge.org\n")

    
    all_files = [short_announce, detailed_announce, wiki_announce]
    write_all(all_files, "Subject: %s\n\n" % subject)
    write_all(all_files, announce+"\n")
    write_all(all_files, "Download\n--------\n\nhttp://www.worldforge.net/aloril/cyphesis/cyphesis-%s.tar.gz\n\n" % version)
    short_announce.write("For more info about release see mail to server@worldforge.org list.\n")
    detailed_announce.write(creation_text+"\n\n")
    detailed_announce.write("ChangeLog:\n----------\n\n" + change_log)

    short_announce.close()
    detailed_announce.close()
    wiki_announce.close()
