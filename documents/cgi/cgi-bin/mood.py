#!/usr/bin/python3

import os
import sys
import urllib.parse

print("Content-Type: text/html; charset=utf-8\r\n")
print()  # Required blank line after headers

method = os.environ.get("REQUEST_METHOD", "GET")
mood = ""
post_data = ""

# Only read stdin if it's a POST request
if method == "POST":
	content_length = int(os.environ.get("CONTENT_LENGTH", 0))
	if content_length > 0:
		post_data = sys.stdin.read(content_length)
		params = urllib.parse.parse_qs(post_data)
		mood = params.get("mood", [""])[0]

# Begin HTML response
print("""
<!DOCTYPE html>
<html>
<head>
	<meta charset="UTF-8">
	<title>Select Your Mood</title>
	<style>
		body {
			background-color: #0c0f1a;
			color: #00ffe7;
			font-family: monospace;
			display: flex;
			flex-direction: column;
			align-items: center;
			justify-content: center;
			height: 100vh;
			margin: 0;
		}
		form {
			margin: 20px;
		}
		button {
			font-size: 5rem;
			padding: 10px;
			margin: 5px;
			cursor: pointer;
			border: none;
			background: transparent;
		}
	</style>
</head>
<body>
	<h1>How are you feeling today?</h1>
	<form method="POST" action="/cgi-bin/mood.py">
		<button type="submit" name="mood" value="😊">😊</button>
		<button type="submit" name="mood" value="😐">😐</button>
		<button type="submit" name="mood" value="😢">😢</button>
		<button type="submit" name="mood" value="😡">😡</button>
	</form>
""")

# If a mood was submitted, show a message
if mood:
	print(f"<h2>You feel: {mood}</h2>")

print("</body></html>")
