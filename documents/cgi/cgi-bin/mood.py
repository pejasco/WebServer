#!/usr/bin/python3

import os
import sys
import urllib.parse

print("Content-Type: text/html; charset=utf-8")

method = os.environ.get("REQUEST_METHOD", "GET")
mood = ""
post_data = ""

# we onlyyy read stdin if it's a POST request
if method == "POST":
	content_length = int(os.environ.get("CONTENT_LENGTH", 0))
	if content_length > 0:
		post_data = sys.stdin.read(content_length)
		params = urllib.parse.parse_qs(post_data)
		mood = params.get("mood", [""])[0]
		print(f"Content-Length:  {content_length}")

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
		.method {
			font-size: 0.8rem;
			color: #ffae00;
			margin-top: 6px;
		}
		.homepage {
			position: absolute;
			top: 10px;
			left: 10px;
			padding: 10px 16px;
			background-color: #003344;
			color: #00ffe7;
			border: 1px solid #00ffe7;
			border-radius: 8px;
			font-size: 1rem;
			cursor: pointer;
		}
		#emoji-container {
			position: fixed;
			top: 0;
			left: 0;
			width: 100%;
			height: 100%;
			z-index: 999;
			pointer-events: none;
		}
		.emoji {
			position: absolute;
			font-size: 2.5rem;
			animation: fall linear;
		}
		@keyframes fall {
			0% { transform: translateY(-100px); opacity: 1;}
			100% { transform: translateY(100vh); opacity: 0.7; }
		}
	</style>
</head>
<body>
	<h1>How are you feeling today?</h1>
	<div class="method">POST</div>
	<form method="POST" action="mood.py">
		<button type="submit" name="mood" value="😊">😊</button>
		<button type="submit" name="mood" value="😐">😐</button>
		<button type="submit" name="mood" value="😢">😢</button>
		<button type="submit" name="mood" value="😡">😡</button>
		<button type="submit" name="mood" value="🥰">🥰</button>
	</form>
	<button class="homepage" onclick="window.location.href='http://localhost:3501/cgi'">Homepage</button>
	<div id="emoji-container"></div>
""", end='')

if mood:
	print(f"<h2>You feel: {mood}</h2>")
	print(f"""
<script>
       const emoji = "{mood}";
       const container = document.getElementById("emoji-container");
       for (let i = 0; i < 350; i++)
       {{
			let span = document.createElement("span");
			span.className = "emoji";
			span.textContent = emoji;
			span.style.left = Math.random() * 800 + "vw";
			span.style.top = Math.random() * -800 + "px";
			span.style.animationDuration = "2s"
			container.appendChild(span);
			setTimeout(() => span.remove(), 3000);
		}}
</script>
</body>
</html>
""", end='')
else:
	print("</body></html>")
