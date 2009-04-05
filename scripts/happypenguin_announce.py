#!/usr/bin/python

import cookielib
import httplib
import re
import sys
import urllib
import urllib2

# <form action="login.php" method="post" target="_top">
# <input type="text" class="post" name="username" size="25" maxlength="40" value="" />
# <input type="password" class="post" name="password" size="25" maxlength="32" />
# <input type="hidden" name="autologin" value="on"/></span>
# <input type="hidden" name="redirect" value="" />
# <input type="submit" name="login" class="mainoption" value="Log in" />

# username=wef&password=wef&autologin=on&redirect=&login=Log+in

_LOGIN_FORM={
    'autologin': 'on',
    'redirect': '',
    'login': 'Log in'
}

_UPDATE_FORM={
'title': 'WorldForge::Cyphesis',
'author': 'Al Riddoch',
'summary': '',
'authoremail': 'alriddoch@googlemail.com',
'company': '',
'homepage': 'http://www.worldforge.org/dev/eng/servers/cyphesis',
'download': 'http://downloads.sourceforge.net/worldforge/cyphesis-0.5.18.tar.bz2',
'url1': 'http://downloads.sourceforge.net/worldforge/cyphesis-0.5.18.package',
'url1description': 'Linux autopackage',
'url2': 'http://worldforge.org/dev/eng/clients/sear',
'url2description': 'WorldForge Sear Client',
'url3': 'http://worldforge.org/dev/eng/clients/ember',
'url3description': 'WorldForge Ember Client',
'version': '0.5.19',
'short_description': 'MMORPG server for the WorldForge project with embedded AI.',
'long_description': '<p>Cyphesis is a fantasy MMORPG server, and NPC engine for servers, using AI/A-Life techniques which doesn\'t have a predefined story. It is the Artificial Intelligence and Artificial Life server/client used by the WorldForge project.</p>',
'category': 'rpg',
'license': 'free',
'cost': '',
'source': 'on',
'x11': 'on',
'console': 'on',
'multiplayer': 'on',
'network': 'on',
'other_requirements': '<ul>%0D%0A<li>WorldForge::Atlas-C++</li>%0D%0A<li>WorldForge::skstream</li>%0D%0A<li>WorldForge::wfmath</li>%0D%0A<li>WorldForge::Mercator</li>%0D%0A<li>WorldForge::varconf</li>%0D%0A<li>PostgreSQL</li>%0D%0A<li>Python</li>%0D%0A<li>readline</li>%0D%0A<li>HOWL or Avahi</li>%0D%0A<li>GCrypt</li>%0D%0A</ul>',
'submit': 'Submit',
}

_USERNAME='alriddoch'

def get_using_urllib(password):
    cj = cookielib.CookieJar()
    opener = urllib2.build_opener(urllib2.HTTPCookieProcessor(cj))
    login_form = _LOGIN_FORM.copy()
    login_form['username'] = _USERNAME
    login_form['password'] = password
    # body = "username=%s&password=%s&autologin=on&redirect=&login=Log+in" % (_USERNAME, password)
    body = urllib.urlencode(login_form)
    try:
        res = opener.open('http://happypenguin.org/forums/login.php', body)
    except urllib2.URLError, e:
        print "Unable to open"
        return
    content = res.read()

    if re.search('incorrect or inactive username', content):
        print 'login failed'
    elif re.search('You are not allowed to login', content):
        print 'login banned'
    elif re.search('Log out \[ %s \]' % _USERNAME, content):
        print 'logged in'
    else:
        print 'dunno'
        print content
        return

    res = opener.open('http://happypenguin.org/')
    content = res.read()
    print len(content)

def get_using_httplib(password):
    con = httplib.HTTPConnection('happypenguin.org')

    body = "username=%s&password=%s&autologin=on&redirect=&login=Log+in" % (_USERNAME, password)
    headers = {}
    headers['Content-Type'] = 'application/x-www-form-urlencoded'
    con.request('POST', '/forums/login.php', body, headers)
    res = con.getresponse()
    content = res.read()
    headers = res.getheaders()
    print res.status
    print content
    print headers

    for h in headers:
      if len(h) < 2:
        continue
      if h[0] == 'set-cookie':
        print "COOKIE", h[1]

def Main():
    if len(sys.argv) != 2:
        print "usage: announce <password>"
        exit(1)
    get_using_urllib(sys.argv[1])

if __name__ == '__main__':
    Main()
