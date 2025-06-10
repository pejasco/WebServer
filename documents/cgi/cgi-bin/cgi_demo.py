#!/usr/bin/python3

body = """
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title>CGI Demo</title>
	<style>
		body {
			background-color: #1e1e2f;
			color: #f0f0f0;
			font-family: "Courier New", monospace;
			display: flex;
			flex-direction: column;
			align-items: center;
			padding-top: 50px;
		}
		h1 {
			color: #80ffdb;
			font-size: 2.5em;
		}
		p {
			color: #a0c4ff;
			margin-top: 10px;
		}
		.box {
			background: #2d2d44;
			padding: 20px;
			border-radius: 12px;
			box-shadow: 0 0 10px rgba(128, 255, 219, 0.2);
		}
	</style>
</head>
<body>
	<div class="box">
		<h1>Hello from Python CGI!</h1>
		<p>Your server is officially running dynamic scripts 🔥</p>
	</div>
</body>
</html>
"""

print("Content-Type: text/html")
print()
print(body)
