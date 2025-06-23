#!/usr/bin/env python3

import random
import html

print("Content-Type: text/html; charset=utf-8")

memes = [
	"https://s3.amazonaws.com/rails-camp-tutorials/blog/programming+memes/works-doesnt-work.jpg",
	"https://plaky.com/blog/wp-content/uploads/2023/08/Intro.jpg",
	"https://plaky.com/blog/wp-content/uploads/2023/08/It-was-me.jpg",
	"https://plaky.com/blog/wp-content/uploads/2023/08/The-bell-curve.jpg",
	"https://plaky.com/blog/wp-content/uploads/2023/08/Chat-GPT-programming-language.jpg",
	"https://i.pinimg.com/736x/ca/2e/5c/ca2e5c31ef489f40504f56b0626562eb.jpg",
	"https://i.pinimg.com/736x/89/40/18/89401895a6dea6456dd00e801466650f.jpg",
	"https://i.pinimg.com/736x/4f/a8/1f/4fa81f2676b075a89ff6397318b7c8d2.jpg",
	"https://i.pinimg.com/736x/f2/af/53/f2af53e4f59d5db11d7252e9787338fc.jpg",
	"https://i.pinimg.com/736x/d7/eb/93/d7eb936a6df7bb401eb865b0627f61e1.jpg",
	"https://plaky.com/blog/wp-content/uploads/2023/08/Code-doesnt-work.jpg",
	"https://i.pinimg.com/736x/c8/2e/23/c82e23d3b4d7feb64e9ac2175f7fa07f.jpg",
	"https://media.giphy.com/media/v1.Y2lkPTc5MGI3NjExZ2tqdHgwcW51dTVlZ20zbmtyazN4bDR0bjlkcjA1M2llZ3JoNzlnYyZlcD12MV9naWZzX3NlYXJjaCZjdD1n/pUVOeIagS1rrqsYQJe/giphy.gif",
	"https://media.giphy.com/media/v1.Y2lkPTc5MGI3NjExZ2tqdHgwcW51dTVlZ20zbmtyazN4bDR0bjlkcjA1M2llZ3JoNzlnYyZlcD12MV9naWZzX3NlYXJjaCZjdD1n/oaDcc0LTCuIAiGYrzn/giphy.gif",
	"https://media.giphy.com/media/v1.Y2lkPTc5MGI3NjExNXVlMnBrbDZ5eXNsb2Y1NXcyaGtsa3VscjhrN25ubXE5ZWEwOWIwdSZlcD12MV9naWZzX3NlYXJjaCZjdD1n/lgcUUCXgC8mEo/giphy.gif"
]

chosen_meme = random.choice(memes)

print(f"""
<!DOCTYPE html>
<html>
<head>
	<meta charset="UTF-8">
	<title>Random Meme Generator</title>
	<style>
		body {{
			background-color: #0c0f1a;
			color: #7fffd4;
			font-family: monospace;
			display: flex;
			flex-direction: column;
			align-items: center;
			justify-content: center;
			height: 100vh;
			margin: 0;
		}}
		img {{
			max-width: 85%;
			max-height: 75vh;
			border: 3px solid #00ffe7;
			border-radius: 10px;
			box-shadow: 0 0 10px #00ffe7;
			margin-bottom: 20px;
		}}
		button {{
			padding: 12px 20px;
			background-color: #003344;
			color: #00ffe7;
			border: 1px solid #00ffe7;
			border-radius: 8px;
			font-size: 1rem;
			cursor: pointer;
		}}
		.method {{
			font-size: 0.8rem;
			color: limegreen;
			margin-top: 6px;
		}}
		.homepage {{
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
		}}
	</style>
</head>
<body>
	<img src="{html.escape(chosen_meme)}" alt="Random Meme">
	<form method="GET">
		<button type="submit">🎲 Shuffle meme!</button>
	</form>
	<div class="method">GET</div>
	<button class="homepage"<button onclick="window.location.href='http://localhost:3501/cgi'">Homepage</button></button>
</body>
</html>
""", end='')
