import requests
import tempfile
import socket
import urllib3
import os
from requests.adapters import HTTPAdapter
from urllib3.util.retry import Retry

print("\n=====================================")
print("BASIC TEST FOR METHOD GET, POST, DELETE")
print("=====================================\n")
'''
#Basic GET request
try:
	get_check = requests.get('http://localhost:8080', timeout=5, stream=False)
	print(f"[GET] => ✅ Success! Status Code: {get_check.status_code}")
except Exception as e:
	print(f"[GET] => ❌ failed: {e}")

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
print("\n======================================")
print("BASIC TEST FOR METHOD GET, POST with CGI")
print("======================================\n")

# GET for CGI
print(" -- GET with CGI -- ")
print("## SCRIPT with py")
try:
	response = requests.get(
		'http://localhost:3501/cgi/cgi-bin/birthday.py?day=6&month=4',
		timeout=5,
		stream=False  # Don't stream the response
	)       
	print(f"[GET CGI] => ✅ Success! Status: {response.status_code}")
except Exception as e:
	print(f"[GET CGI] => ❌ Failed: {e}")

try:
	get_cgi_check = requests.get('http://localhost:3501/cgi/cgi-bin/meme.py', timeout=5)
	print(f"[GET CGI] => ✅ Success! Status: {get_cgi_check.status_code}")
except Exception as e:
	print(f"[GET CGI] => ❌ failed: {e}")

try:
	get_cgi_check = requests.get('http://localhost:3501/cgi/cgi-bin/env.py', timeout=5)
	print(f"[GET CGI] => ✅ Success! Status: {get_cgi_check.status_code}")
except Exception as e:
	print(f"[GET CGI] => ❌ failed: {e}")

print("## SCRIPT with sh")
try:
	get_cgi_check = requests.get('http://localhost:3501/cgi/cgi-bin/webserv.sh', timeout=5)
	print(f"[GET CGI] => ✅ Success! Status: {get_cgi_check.status_code}")
except Exception as e:
	print(f"[GET CGI] => ❌ failed: {e}")

# POST for CGI
print(" -- POST with CGI -- ")
try:
	data = {'name': 'username'}
	post_cgi_check = requests.post('http://localhost:3501/cgi/cgi-bin/say_hello.py', data=data, timeout=5)
	print(f"[POST CGI] => ✅ Success! Status: {post_cgi_check.status_code}")
except Exception as e:
	print(f"[POST CGI] => ❌ failed: {e}")

try:
	data = {'mood': "%F0%9F%98%8A"}
	post_cgi_check = requests.post('http://localhost:3501/cgi/cgi-bin/mood.py', data=data, timeout=5)
	print(f"[POST CGI] => ✅ Success! Status: {post_cgi_check.status_code}")
except Exception as e:
	print(f"[POST CGI] => ❌ failed: {e}")


# test of Error handling
print("\n===========")
print("ERROR TESTING")
print("===========\n")

print(" -- Config File Error --")
print( "In progress...")

print(" -- HTTP error code matching")
try:
	get_error_check = requests.get('http://localhost:8080/tanzania/home.html', timeout=5)
	if (get_error_check.status_code == 404):
		print(f"[ERROR -- wrong file name] => ✅ Success! Status: {get_error_check.status_code}")
	else:
		print(f"[ERROR -- wrong file name] => ❌ failed: Error code incorrect: {get_error_check.status_code}")
except Exception as e:
	print(f"[ERROR -- wrong file name] => ❌ failed: {e}")

try:
	get_error_check = requests.get('http://localhost:9000/upload/up.html', timeout=5)
	if (get_error_check.status_code == 404):
		print(f"[ERROR -- wrong file name] => ✅ Success! Status: {get_error_check.status_code}")
	else:
		print(f"[ERROR -- wrong file name] => ❌ failed: Error code incorrect: {get_error_check.status_code}")
except Exception as e:
	print(f"[ERROR -- wrong file name] => ❌ failed: {e}")

try:
	get_error_check = requests.delete('http://localhost:8080', timeout=5)
	if (get_error_check.status_code == 405):
		print(f"[ERROR -- method not allow] => ✅ Success! Status: {get_error_check.status_code}")
	else:
		print(f"[ERROR -- method not allow] => ❌ failed: Error code incorrect: {get_error_check.status_code}")
except Exception as e:
	print(f"[ERROR -- method not allow] => ❌ failed: {e}")

try:
	get_error_check = requests.delete('http://localhost:8080/tanzania/homepage.html', timeout=5)
	if (get_error_check.status_code == 405):
		print(f"[ERROR -- method not allow] => ✅ Success! Status: {get_error_check.status_code}")
	else:
		print(f"[ERROR -- method not allow] => ❌ failed: Error code incorrect: {get_error_check.status_code}")
except Exception as e:
	print(f"[ERROR -- method not allow] => ❌ failed: {e}")


try:
	with tempfile.NamedTemporaryFile(delete=False) as tmp_file:
		# Write 50MB of data
		chunk = b'F' * (1024 * 1024)  # 1MB chunk
		for _ in range(50):  # Write 50 chunks = 50MB
			tmp_file.write(chunk)
		tmp_file_path = tmp_file.name
	
	with open(tmp_file_path, 'rb') as f:
		files = {'file': ('large_file.txt', f, 'text/plain')}
		get_error_check = requests.post('http://localhost:9000/upload', 
								   files=files, 
								   timeout=60)
	if (get_error_check.status_code == 413):
		print(f"[ERROR -- file too large] => ✅ Success! Status: {get_error_check.status_code}")
	else:
		print(f"[ERROR -- file too large] => ❌ failed: Error code incorrect: {get_error_check.status_code}")
except requests.exceptions.ConnectionError as e:
	error_str = str(e)
	if "Connection aborted" in error_str and "104" in error_str:
		print("[ERROR -- file too large] => ✅ Success! Connection reset as usual per Python Client after 413 error code")
	else:
		print(f"Connection error: {e}")
except Exception as e:
	print(f"[POST upload -- file too large] => ❌ failed: {e}")

try:
	huge_value = 'H' * 20000 
	get_error_check = requests.get('http://localhost:9000/upload/', headers={'X-Large-Header': huge_value},timeout=5)
	if (get_error_check.status_code == 431):
		print(f"[ERROR -- header too large] => ✅ Success! Status: {get_error_check.status_code}")
	else:
		print(f"[ERROR]  -- header too large => ❌ failed: Error code incorrect: {get_error_check.status_code}")
except requests.exceptions.ConnectionError as e:
	# This is actually expected for 413 errors!
	if "Connection reset by peer" in str(e):
		print(f"[ERROR -- header too large] => ✅ Success! Status: {get_error_check.status_code}")
	else:
		print(f"[ERROR -- header too large] => ❌ Connection error: {e}")
except Exception as e:
	print(f"[ERROR  -- header too large] => ❌ failed: {e}")

print(" -- Timeout -- ")
try:
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(15)  # Wait longer than server timeout
    sock.connect(('localhost', 8080))
    
    # Send incomplete request
    incomplete_request = b"GET /test HTTP/1.1\r\n"
    sock.send(incomplete_request)
    
    # Wait for server timeout
    response = sock.recv(1024).decode('utf-8', errors='ignore')
    sock.close()
    
    if "408" in response:
        print(f"[ERROR 408 -- Request timeout] => ✅ Success! Response contains 408")
    else:
        print(f"[ERROR 408 -- Request timeout] => ❌ failed: Response: {response[:100]}")
        
except socket.timeout:
    print(f"[ERROR 408 -- Request timeout] => ❌ failed: Socket timed out before server responded")
except Exception as e:
    print(f"[ERROR 408 -- Request timeout] => ❌ failed: {e}")

print(" -- Default error page -- ")
'''
try:
    get_error_check = requests.post('http://localhost:9000/upload', 
                                   data="test data",
                                   headers={'Content-Type': 'application/x-fake-type'}, 
                                   timeout=5)
    if (get_error_check.status_code == 500):
        print(f"[ERROR -- Unsupported media type] => ✅ Success! Using the default error page Status: {get_error_check.status_code}")
    else:
        print(f"[ERROR -- Unsupported media type] => ❌ failed: Error code incorrect: {get_error_check.status_code}")
except Exception as e:
    print(f"[ERROR -- Unsupported media type] => ❌ failed: {e}")

print(" -- Error on CGI execution -- ")
try:
	get_error_check = requests.get('http://localhost:3501/cgi/cgi-bin/fail_500.py', timeout=5)
	if (get_error_check.status_code == 500):
		print(f"[ERROR CGI -- CGI crash] => ✅ Success! Status: {get_error_check.status_code}")
	else:
		print(f"[ERROR CGI -- CGI crash] => ❌ failed: Error code incorrect: {get_error_check.status_code}")
except Exception as e:
	print(f"[ERROR CGI -- CGI crash] => ❌ failed: {e}")

try:
	get_error_check = requests.get('http://localhost:3501/cgi/cgi-bin/color.py', timeout=5)
	if (get_error_check.status_code == 404):
		print(f"[ERROR CGI-- unexisting file] => ✅ Success! Status: {get_error_check.status_code}")
	else:
		print(f"[ERROR CGI -- unexisting file] => ❌ failed: Error code incorrect: {get_error_check.status_code}")
except Exception as e:
	print(f"[ERROR CGI -- unexisting file] => ❌ failed: {e}")
 
# Stress test 

# Use another script to create fake config file to use to test server setup 





# OLD

'''
def test_large_file_urllib3():
	"""Test using urllib3 for more control over the request"""
	http = urllib3.PoolManager()
	
	try:
		with tempfile.NamedTemporaryFile(delete=False) as tmp_file:
			chunk = b'F' * (1024 * 1024)
			for _ in range(50):
				tmp_file.write(chunk)
			tmp_file_path = tmp_file.name
		
		# Read file as bytes
		with open(tmp_file_path, 'rb') as f:
			file_data = f.read()
		
		# urllib3 with bytes data
		response = http.request('POST', 'http://localhost:8080/upload',
							  fields={'file': ('large_file.txt', file_data, 'text/plain')},
							  timeout=60)
		
		print(f"[ERROR -- file too large] => ✅ Status: {response.status}")
		return response.status
		
	except urllib3.exceptions.ProtocolError as e:
		# Check if the error message contains 413 or indicates payload too large
		error_msg = str(e)
		if "413" in error_msg or "Payload Too Large" in error_msg:
			print(f"[ERROR -- file too large] => ✅ Protocol error (413 detected): {error_msg}")
			return 413
		else:
			print(f"[ERROR -- file too large] => Protocol error (other): {error_msg}")
			return "protocol_error_other"
			
	except urllib3.exceptions.MaxRetryError as e:
		error_msg = str(e)
		if "413" in error_msg or "Payload Too Large" in error_msg:
			print(f"[ERROR -- file too large] => ✅ Max retry error (413 detected): {error_msg}")
			return 413
		else:
			print(f"[ERROR -- file too large] => Max retry error (other): {error_msg}")
			return "max_retry_other"
	except Exception as e:
		print(f"[ERROR -- file too large] => ❌ urllib3 failed: {e}")
		return None
	finally:
		if 'tmp_file_path' in locals():
			os.unlink(tmp_file_path)
print(test_large_file_urllib3())'''