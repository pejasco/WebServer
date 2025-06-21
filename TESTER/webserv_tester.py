import requests

#Basic GET request
#get_check = requests.get('http://localhost:8080')
#print(f"GET check:\nStatus Code: {get_check.status_code}")

#Basic POST request
#post_check = requests.post('http://localhost:3501')
#print(f"POST check:\nStatus Code: {post_check.status_code}")

#Basic DELETE request
delete_check = requests.delete('http://localhost:8080')
print(f"DELETE check:\nStatus code: {delete_check.status_code}")

