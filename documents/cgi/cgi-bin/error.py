#!/usr/bin/env python3

print("HTTP/1.1 200 OK")
print("Content-Type: text/html")
print("</body>")
print("</html>")

<<<<<<< Updated upstream:documents/cgi/cgi-bin/error.py
#print(1 / 0)
=======
# print(1 / 0)
>>>>>>> Stashed changes:documents/cgi/cgi-bin/divzero.py
#This wont work but wont crash :)

while True:
    pass

# raise Exception("This is a test crash for 500 error.")
#stimulating crashhh