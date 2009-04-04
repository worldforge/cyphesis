#!/usr/bin/python

import httplib

# <form action="login.php" method="post" target="_top">
# <input type="text" class="post" name="username" size="25" maxlength="40" value="" />
# <input type="password" class="post" name="password" size="25" maxlength="32" />
# <input type="hidden" name="autologin" value="on"/></span>
# <input type="hidden" name="redirect" value="" />
# <input type="submit" name="login" class="mainoption" value="Log in" />

# username=wef&password=wef&autologin=on&redirect=&login=Log+in

_USERNAME='alriddoch'
_PASSWORD=''

def Main():
    con = httplib.HTTPConnection('happypenguin.org')
    # con = httplib.HTTPConnection('localhost', 8080)
    # con = httplib.HTTPConnection('http://happypenguin.org/forums/login.php')
    body = "username=%s&password=%s&autologin=on&redirect=&login=Log+in" % (_USERNAME, _PASSWORD)
    con.request('POST', '/forums/login.php', body)
    res = con.getresponse()
    content = res.read()
    print res.status
    print content
    print 'foo'

if __name__ == '__main__':
    Main()
