#!/usr/bin/env python

import cgi

# Get the form data
form = cgi.FieldStorage()

# Get the value of the 'first_name' and 'last_name' fields
first_name = form.getvalue('first_name')
last_name = form.getvalue('last_name')

# Print the submitted name
print("First Name: {}".format(first_name))
print("Last Name: {}".format(last_name))
