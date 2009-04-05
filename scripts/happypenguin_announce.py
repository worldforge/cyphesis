#!/usr/bin/python

import cookielib
import httplib
import re
import sys
import urllib2

# <form action="login.php" method="post" target="_top">
# <input type="text" class="post" name="username" size="25" maxlength="40" value="" />
# <input type="password" class="post" name="password" size="25" maxlength="32" />
# <input type="hidden" name="autologin" value="on"/></span>
# <input type="hidden" name="redirect" value="" />
# <input type="submit" name="login" class="mainoption" value="Log in" />

# username=wef&password=wef&autologin=on&redirect=&login=Log+in

_USERNAME='alriddoch'

def get_using_urllib(password):
    cj = cookielib.CookieJar()
    opener = urllib2.build_opener(urllib2.HTTPCookieProcessor(cj))
    body = "username=%s&password=%s&autologin=on&redirect=&login=Log+in" % (_USERNAME, password)
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
    print content

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
