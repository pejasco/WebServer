#!/usr/bin/python3

#write a simple script asking the usr his name
#then print "Hello <name>!"

import os
import sys
import urllib.parse

method = os.environ.get('REQUEST_METHOD', 'GET')
name = ""
post_data = ""

if method == 'POST':
	content_length = int(os.environ.get('CONTENT_LENGTH', 0))
	if content_length > 0:
		post_data = sys.stdin.read(content_length)
		params = urllib.parse.parse_qs(post_data)
		name = params.get('name', [''])[0].capitalize()

body = """
<!DOCTYPE html>
<html>
<head>
	<title>Say Hello (POST)</title>
	<style>
		body {
			background-color: #0c0f1a;
			color: white;
			font-family: 'Segoe UI', sans-serif;
			display: flex;
			flex-direction: column;
			align-items: center;
			justify-content: center;
			height: 100vh;
		}
		input[type="text"], input[type="submit"] {
			margin: 10px;
			padding: 10px;
			border-radius: 5px;
		}
	</style>
</head>
<body>
	<form method="POST" action="/cgi/cgi-bin/say_hello.py">
		<input type="text" name="name" placeholder="Enter your name" />
		<input type="submit" value="Say Hello" />
	</form>
"""

# Append the greeting if there's a name
if name:
	body += f"<h1>Hello cadet {name}! POST works!</h1>"

# Final closing tags + debug
body += "</body></html>"
body += f"<p>[DEBUG] Post Data Raw: {post_data}</p>"

# Required headers
print("Content-Type: text/html")
print(f"Content-Length: {len(body.encode('utf-8'))}")
print()  # blank line after headers
print(body)
