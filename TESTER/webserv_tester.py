import requests
from requests.adapters import HTTPAdapter
from urllib3.util.retry import Retry

print("BASIC TEST FOR METHOD GET, POST, DELETE\n")

#Basic GET request
try:
	get_check = requests.get('http://localhost:8080', timeout=5, stream=False)
	print(f"[GET] => ✅ Success! Status Code: {get_check.status_code}")
except Exception as e:
	print(f"[GET] => ❌ failed: {e}")

# GET with CGI    
try:
    response = requests.get(
        'http://localhost:3501/cgi/cgi-bin/birthday.py?day=6&month=4',
        timeout=5,
        stream=False  # Don't stream the response
    )       
    print(f"[GET CGI] => ✅ Success! Status: {response.status_code}")
except Exception as e:
    print(f"[GET CGI] => ❌ Failed: {e}")

#Basic POST request

# POST for upload
try:
	with open('test.txt', 'w') as f:
		f.write('This is an upload test file for POST request')
	with open('test.txt', 'rb') as f:
		file = {'file': f}
		post_check = requests.post('http://localhost:9000/upload', files=file, timeout=5)
	print(f"[POST upload] => ✅ Success! Status Code: {post_check.status_code}")
except Exception as e:
	print(f"[POST upload] => ❌ failed: {e}")

# POST for CGI
try:
	data = {'name': 'username'}
	post_check = requests.post('http://localhost:3501/cgi/cgi-bin/say_hello.py', data=data, timeout=5)
	print(f"[POST CGI] => ✅ Success! Status: {response.status_code}")
except Exception as e:
	print(f"[POST CGI] => ❌ failed: {e}")

#Basic DELETE request
try:
	file = 'test.txt'
	delete_check = requests.delete(f'http://localhost:9000/upload/storage/{file}', timeout=5)	
	print(f"[DELETE] => ✅ Success! Status code: {delete_check.status_code}")
except Exception as e:
	print(f"[DELETE] => ❌ failed: {e}")

print()

###OTHER TESTS TO CREATE###

# Test of CGI 

# test of Error handling

# Stress test 

# Use another script to create fake config file to use to test server setup 
