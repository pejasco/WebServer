#!/usr/bin/python3

#write a simple script asking the usr his name
#then print "Hello <name>!"

#will handle POST

import os
import sys
import urllib.parse
import time

time.sleep(3)
content_length = int(os.environ.get('CONTENT_LENGTH', 0))
query_string = os.environ.get('QUERY_STRING', '')


params = urllib.parse.parse_qs(query_string)
name = params.get('name', [''])[0].capitalize()

print("Content-Type: text/html")
print()
print(f"<html><body><h1>Hello cadet {name}! Welcome to WebServ!</h1></body></html>")
