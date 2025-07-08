#!/usr/bin/python3

#write a simple script asking the usr his name
#then print "Hello <name>!"

import os
import sys
import urllib.parse

method = os.environ.get('REQUEST_METHOD', 'GET')
name = ""
post_data = ""

#Better to calculate the body size just before sending the response as can be mismatch character from python to C++
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
			color: #7fffd4;
			font-family: monospace;
			display: flex;
			flex-direction: column;
			align-items: center;
			justify-content: center;
			height: 100vh;
			margin: 0;
		}
		img {
			max-width: 85%;
			max-height: 75vh;
			border: 3px solid #00ffe7;
			border-radius: 10px;
			box-shadow: 0 0 10px #00ffe7;
			margin-bottom: 20px;
		}
		button {
			padding: 12px 20px;
			background-color: #003344;
			color: #00ffe7;
			border: 1px solid #00ffe7;
			border-radius: 8px;
			font-size: 1rem;
			cursor: pointer;
		}
		input[type="text"], input[type="submit"] {
			margin: 10px;
			padding: 10px;
			border-radius: 5px;
		}
		input[type="text"] {
			margin: 10px;
			padding: 10px;
			background-color: #003044;
			color: #00ffe7;
			border: 1px solid #00ffe7;
			border-radius: 8px;
		}
		input[type="submit"] {
			padding: 12px 20px;
			background-color: #003344;
			color: #00ffe7;
			border: 1px solid #00ffe7;
			border-radius: 8px;
			font-size: 1rem;
			cursor: pointer;
		}
		.homepage {
			position: absolute;
			top: 10px;
			left: 10px; /* Or use right: 10px; for right side */
			padding: 10px 16px;
			background-color: #003344;
			color: #00ffe7;
			border: 1px solid #00ffe7;
			border-radius: 8px;
			font-size: 1rem;
			cursor: pointer;
		}
		.method {
			font-size: 0.8rem;
			color: #ffae00;
			margin-top: 6px;
		}
		.wave {
			position: absolute;
			bottom: 10px;
			width: 420px;
		}
	</style>
</head>
<body>
	<h1>Hellooo whats your name?</h1>
	<div class="method">POST</div>
	<form method="POST" action="say_hello.py">
		<input type="text" name="name" placeholder="Enter your name" />
		<input type="submit" value="Say Hello" />
	</form>
	<button class="homepage" onclick="window.location.href='http://localhost:3501/cgi'">Homepage</button>
"""

# append the greeting if there's a name
if name:
	body += f"<h1>Hello cadet {name}! POST works!</h1>"
	body += f"<img src='https://media1.giphy.com/media/v1.Y2lkPTc5MGI3NjExcWY5YW5nbjZ0eXRqeTJpYWZpbjRocHZvemdiM2VqYTIwN2c0bXhxYyZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/IThjAlJnD9WNO/giphy.gif' class='wave' alt='WAVE' />"

body += "</body></html>"
# body += f"<p>[DEBUG] Post Data Raw: {post_data}</p>"

print("Content-Type: text/html")
#print(f"Content-Length: {len(body.encode('utf-8'))}")
print()
print(body)
