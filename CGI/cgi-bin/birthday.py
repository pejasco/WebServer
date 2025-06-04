#!/usr/bin/env python3

import cgi
from datetime import datetime, date

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
	<link rel="stylesheet" href="cgi-bin.css">
</head>
<body>
	<h1>🎂 Birthday Countdown 🎉</h1>
""")

if day and month:
	try:
		days_left = calculate_days_left(month, day)
		print(f"<p>Your birthday is in <strong>{days_left} day(s)</strong>!</p>")
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
