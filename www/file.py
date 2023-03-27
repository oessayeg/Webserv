#!/usr/bin/env python

import cgi

form = cgi.FieldStorage()

name = form.getvalue('name')
email = form.getvalue('email')

print("<html>")
print("<body>")
print("<h1>Data submitted from the form:</h1>")
print("<p>Name: %s</p>" % name)
print("<p>Email: %s</p>" % email)
print("</body>")
print("</html>")
