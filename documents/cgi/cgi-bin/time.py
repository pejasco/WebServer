#!/usr/bin/python3

import datetime

print("HTTP/1.1 200 OK")
print("Content-Type: text/html\r\n")

now = datetime.datetime.now().strftime("%H:%M:%S")

print(f"""
<html>
<head>
	<meta http-equiv="refresh" content="1">
	<title>Live Clock</title>
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
		h1 {{
			font-size: 5rem;
			margin: 0;
			text-shadow: 0 0 10px #00ffe7;
		}}
		.homepage {{
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
			text-decoration: none;
		}}
	</style>
</head>
<body>
    <button class="homepage"<button onclick="window.location.href='http://localhost:3501/cgi'">Homepage</button></button>
	<h1>{now}</h1>
</body>
</html>
""")
