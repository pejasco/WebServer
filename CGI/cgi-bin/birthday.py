#!/usr/bin/env python3

import cgi
import cgitb
from datetime import datetime, date

cgitb.enable()  # For debugging

print("Content-Type: text/html\n")

form = cgi.FieldStorage()
day = form.getfirst("day")
month = form.getfirst("month")

def calculate_days_left(month, day):
	today = date.today()
	this_year = date(today.year, int(month), int(day))
	if this_year < today:
		next_birthday = date(today.year + 1, int(month), int(day))
	else:
		next_birthday = this_year
	delta = next_birthday - today
	return delta.days

print("""
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title>Birthday Countdown</title>
	<style>
		body {
			background-color: #0c0f1a;
			color: #7fffd4;
			font-family: 'Segoe UI', sans-serif;
			text-align: center;
			padding: 40px;
		}
		input, button {
			padding: 10px;
			margin: 10px;
			border-radius: 6px;
			border: 1px solid #7fffd4;
			background-color: #003366;
			color: #7fffd4;
		}
		button:hover {
			background-color: #005580;
		}
		.confetti {
			width: 250px;
			margin-top: 0px;
		}
		.shaq {
			position: absolute;
			right: 10px;
			bottom: 10px;
			width: 420px;
		}
	</style>
</head>
<body>
	<br><br><br><br><br><br><br><br><br>
	<h1>🎂 Birthday Countdown 🎉</h1>
""")

if day and month:
	try:
		days_left = calculate_days_left(month, day)
		print("""<br>""")
		print(f"<h3>Your birthday is in <strong>{days_left} day(s)</strong>!</h3>")
		print("""
		<img src="https://media2.giphy.com/media/RkN33Se0a99r6pMiZi/giphy.gif" class="confetti" alt="Celebration" />
		</div>
		<img src="https://media.giphy.com/media/l0IyaxKjZqenpMIQ8/giphy.gif" class="shaq" alt="Floating cake" />
		""")
	except ValueError:
		print("<p style='color: red;'>Invalid date. Please try again.</p>")
else:
	print("""
	<form method="GET">
		<label for="day">Day (1-31):</label><br>
		<input type="number" name="day" min="1" max="31" required><br>
		<label for="month">Month (1-12):</label><br>
		<input type="number" name="month" min="1" max="12" required><br>
		<button type="submit">Submit</button>
	</form>
	""")

print("""
</body>
</html>
""")