import requests
import tempfile
import socket
import urllib3
import os
import threading
import time
from concurrent.futures import ThreadPoolExecutor
from requests.adapters import HTTPAdapter
from urllib3.util.retry import Retry

'''GET 
function param needed => url, timeout, 
#POST with UPLOAD with filename
#CGI with data
#DELETE with filename
'''

def test_get_request(url, time, filename):
	try:
		if (filename is not None):
			get_check = requests.get(url + filename, timeout=time)
		else:
			get_check = requests.get(url, timeout=time)
		return True, get_check.status_code
	except Exception as e:
		return False, str(e)

def test_upload_request(url, time, filename):
	try:
		if filename is not None:
			upload_check = requests.post(url, files=filename, timeout=time)
		else:
			with open('test.txt', 'w') as f:
				f.write('This is an upload test file for POST request')
			with open('test.txt', 'rb') as f:
				file = {'file': f}
				upload_check = requests.post(url + "/upload", files=file, timeout=time)    
		return True, upload_check.status_code
	except Exception as e:
		return False, str(e)

def test_delete_request(url, time, filename):
	try:
		if (filename is not None):
			delete_check = requests.delete(url + filename, timeout=time)
		else:
			delete_check = requests.delete(url, timeout=time)
		return True, delete_check.status_code
	except Exception as e:
		return False, str(e)

def test_cgi_request(url, time, filename, data):
	try:
		if (data is None):
			cgi_check = requests.get(url + filename, timeout=time)
		else:
			cgi_check = requests.post(url + filename, data=data, timeout=time)
		return True, cgi_check.status_code
	except Exception as e:
		return False, str(e)
	
def test_error_code(url, time, filename, error_code, request_type):
	try:
		full_url = url + filename if filename is not None else url
		methods = {
			"get": requests.get,
			"post": requests.post,
			"delete": requests.delete,
			"put": requests.put,
			"head": requests.head
		}
		method = methods.get(request_type.lower())
		if not method:
			return False, f"Unsupported request type: {request_type}"
		
		error_check = method(full_url, timeout=time)
		
		if error_check.status_code == error_code:
			return True, error_check.status_code
		else:
			return False, error_check.status_code         
	except Exception as e:
		return False, str(e)

def test_error_max_body_size(url, time):
	try:
		with tempfile.NamedTemporaryFile(delete=False) as tmp_file:
			# Write 50MB of data
			chunk = b'F' * (1024 * 1024)  # 1MB chunk
			for _ in range(50):  # Write 50 chunks = 50MB
				tmp_file.write(chunk)
			tmp_file_path = tmp_file.name

		with open(tmp_file_path, 'rb') as f:
			files = {'file': ('large_file.txt', f, 'text/plain')}
			get_error_check = requests.post(url, 
									   files=files, 
									   timeout=time)
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

def test_error_max_header_size(url, time):
	try:
		huge_value = 'H' * 20000 
		get_error_check = requests.get(url, headers={'X-Large-Header': huge_value},timeout=time)
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

def test_error_timeout(url, timeout):
	try:
		sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		sock.connect((url, 8080))
		
		# Send incomplete request
		sock.send(b"GET /test HTTP/1.1\r\nHost: localhost\r\n")
		sock.settimeout(timeout)
		get_error_check = sock.recv(1024).decode()
		if "408" in get_error_check:
			print(f"[ERROR 408 -- Request timeout] => ✅ Success! - Status: 408")
		else:
			print(f"[ERROR 408 -- Request timeout] => ❌ failed")
	except Exception as e:
		print(f"Test failed: {e}")
	finally:
		sock.close()
	
def test_error_default(url, time):
	try:
		long_path = "a" * 8000 + ".html"
		get_error_check = requests.get(url + "/" + long_path, timeout=time)
		if (get_error_check.status_code == 500):
			print(f"[ERROR default -- URI too long (414)] => ✅ Success! Status: {get_error_check.status_code}")
		else:
			print(f"[ERROR default -- URI too long (414)] => ❌ failed: Error code incorrect: {get_error_check.status_code}")
	except Exception as e:
		print(f"[ERROR default -- URI too long (414)] => ❌ failed: {e}")

def test_301_redirect(url, time):
	try:
		response = requests.get(url, timeout=time)
		if response.history:
			for i, redirect in enumerate(response.history):
				if redirect.status_code == 301:
					print(f"[TESTER REDIRECT] => ✅ Success! Status: {redirect.status_code}")
					print(f"   {redirect.url} → {redirect.headers.get('Location')}")
		else:
			print("❌ No redirects occurred")
			
	except Exception as e:
		print(f"❌ Error: {e}")

def individual_test(test_function, *args, **kwargs):
	success, status_or_error = test_function(*args, **kwargs)
	if success:
		print(f"[TESTER] => ✅ Success! Status Code: {status_or_error}")
	else:
		print(f"[TESTER] => ❌ failed: {status_or_error}")

def concurrent_test(test_function, num_concurrent, *args, **kwargs):
	"""Run the same test function multiple times simultaneously"""
	results = []
	def run_single_test():
		return test_function(*args, **kwargs)
	start_time = time.time()

	with ThreadPoolExecutor(max_workers=num_concurrent) as executor:
		futures = [executor.submit(run_single_test) for _ in range(num_concurrent)]
		for future in futures:
			results.append(future.result())
	end_time = time.time()
	
	successful = sum(1 for result in results if result[0] == True)
	total_time = end_time - start_time
	print(f"Concurrent test: {successful}/{num_concurrent} succeeded in {total_time:.2f}s")
	
	# Debug print when not all tests succeed
	if successful != num_concurrent:
		failed_count = num_concurrent - successful
		print(f"🚨 DEBUG: {failed_count} out of {num_concurrent} concurrent tests FAILED")
		failed_results = [result for result in results if result[0] == False]
		print("Failed test details:")
		for i, (success, error_detail) in enumerate(failed_results):
			print(f"  Failed test #{i+1}: {error_detail}")
		
		success_rate = (successful / num_concurrent) * 100
		print(f"Success rate: {success_rate:.1f}%")
		if success_rate < 50:
			print("⚠️  CRITICAL: Less than 50% success rate - possible server blocking or instability!")
		elif success_rate < 80:
			print("⚠️  WARNING: Less than 80% success rate - server may have issues under load")
			
	else:
		print("✅ All concurrent tests passed!")
	
	return successful, num_concurrent, total_time

def test_server_names():
	test_cases = [
		("8080", "WebServer.com", ""),
		("8080", "tanzania", "tanzania/homepage.html"), 
		("9000", "upload", ""),
		("4001", "tooling.com", "tools/welcome.html"),
		("3501", "cgi.com", "cgi-bin/meme.py"),
		("3501", "cgi.com", "cgi-bin/birthday.py?day=6&month=4"),
		("3501", "cgi.com", "cgi-bin/webserv.sh"),
	]
	
	for port, server_name, filename in test_cases:
		try:
			# Send request to localhost:8080 but with different Host header
			headers = {'Host': server_name}
			response = requests.get(f"http://localhost:{port}/{filename}", 
								  headers=headers, timeout=5)
			if (response.status_code == 200):
				print(f"✅ Host: '{server_name}' → Status: {response.status_code}")
				print(f"Webpage sent from server : {response.url}")
			else:
				print(f"❌ Host: '{server_name}' → Status: {response.status_code}")
		except Exception as e:
			print(f"❌ Host: '{server_name}' failed: {e}")

def test_server_multiports():
	test_cases_get = ["8080", "8081", "8082"]
	test_cases_cgi_post = ["3501", "3502", "6000"]

	for port in test_cases_get:
		try:
			response = requests.get(f"http://localhost:{port}", timeout=5)
			if (response.status_code == 200):
				print(f"✅ Port: '{port}' → Status: {response.status_code}")
			else:
				print(f"❌ Port: '{port}' → Status: {response.status_code}")
		except Exception as e:
			print(f"❌ Host: '{port}' failed: {e}")
	for port in test_cases_cgi_post:
		try:
			response = requests.post(f"http://localhost:{port}/cgi-bin/say_hello.py", data={"name": "coco"}, timeout=5)
			if (response.status_code == 200):
				print(f"✅ Port: '{port}' → Status: {response.status_code}")
			else:
				print(f"❌ Port: '{port}' → Status: {response.status_code}")
		except Exception as e:
			print(f"❌ Host: '{port}' failed: {e}")

def test_server_browsers(url, time):
	test_browsers = {
		"Chrome": "Chrome/120.0.0.0 Safari/537.36",
		"Firefox": "Firefox/121.0",
		"Edge": "Edg/120.0.0.0"
	}
	
	for browser, user_agent in test_browsers.items():
		try:
			headers = {"User-Agent": user_agent}
			response = requests.get(url, headers=headers, timeout=time)
			if response.status_code == 200:
				print(f"✅ Browser: '{browser}' → Status: {response.status_code}")
			else:
				print(f"❌ Browser: '{browser}' → Status: {response.status_code}")
		except Exception as e:
			print(f"❌ Browser: '{browser}' failed: {e}")

##################################################
#### UPLOAD AND DELETE TESTING ON CONCURRENCE ####
##################################################

#CREATE THE LIST OF FILE WITH SIMILAR NAME 
def create_test_files(num_files):
	filenames = []
	for i in range(1, num_files + 1):
		filename = f"test{i}.txt"
		with open(filename, 'w') as f:
			f.write(f'Test file #{i} content')
		filenames.append(filename)
	return filenames

def test_concurrent_uploads(url, num_files, timeout):
	print(f"Testing {num_files} concurrent uploads...")
	create_test_files(num_files)
	
	def upload_random_file():
		import random
		file_number = random.randint(1, num_files)
		filename = f"test{file_number}.txt"
		with open(filename, 'rb') as f:
			files = {'file': f}
			time.sleep(random.uniform(0.05, 0.25)) 
			return test_upload_request(url, timeout, files)
	
	successful, total, duration = concurrent_test(upload_random_file, num_files)
	return successful, total, duration

def test_concurrent_deletes(url, num_files, timeout):
	print(f"Testing {num_files} concurrent deletes...")

	def delete_random_file():
		import random
		file_number = random.randint(1, num_files)
		filename = f"/test{file_number}.txt"  # Add / for URL path
		return test_delete_request(url, timeout, filename)
	successful, total, duration = concurrent_test(delete_random_file, num_files)
	return successful, total, duration

def test_upload_and_delete(upload_url, delete_url, num_files, timeout):
	print("=== UPLOAD PHASE ===")
	upload_results = test_concurrent_uploads(upload_url, num_files, timeout)
	print("\n=== DELETE PHASE ===")
	delete_results = test_concurrent_deletes(delete_url, num_files, timeout)
	return upload_results, delete_results

def cleanup_files(num_files):
	for i in range(1, num_files + 1):
		filename = f"test{i}.txt"
		if os.path.exists(filename):
			os.remove(filename)
	print(f"Cleaned up {num_files} test files")


def simple_test():
	print("\n=====================================")
	print("BASIC TEST FOR METHOD GET, POST, DELETE")
	print("=====================================\n")
	individual_test(test_get_request, 'http://localhost:8080', 5, None)
	individual_test(test_upload_request, 'http://localhost:9000/storage', 5, None)
	individual_test(test_delete_request, 'http://localhost:9000', 5, '/storage/test.txt')
	
	print("\n======================================")
	print("BASIC TEST FOR METHOD GET, POST with CGI")
	print("======================================\n")
	individual_test(test_cgi_request, 'http://localhost:3501', 5, '/cgi-bin/birthday.py?day=6&month=4', None)
	individual_test(test_cgi_request, 'http://localhost:3501', 5, '/cgi-bin/meme.py', None)
	individual_test(test_cgi_request, 'http://localhost:3501', 5, '/cgi-bin/env.py', None)
	individual_test(test_cgi_request, 'http://localhost:3501', 5, '/cgi-bin/webserv.sh', None)
	individual_test(test_cgi_request, 'http://localhost:3501', 5, '/cgi-bin/say_hello.py', {'name': 'username'})
	individual_test(test_cgi_request, 'http://localhost:3501', 5, '/cgi-bin/mood.py', {'mood': "%F0%9F%98%8A"})
	
	print("\n===========")
	print("ERROR TESTING")
	print("===========\n")
	individual_test(test_error_code, 'http://localhost:8080', 5, '/tanzania/home.html', 404, "get")
	individual_test(test_error_code, 'http://localhost:9000', 5, '/upload/up.html', 404, "get")
	individual_test(test_error_code, 'http://localhost:8080', 5, None, 405, "delete")
	individual_test(test_error_code, 'http://localhost:8080', 5, '/tanzania/homepage.html', 405, "delete")
	individual_test(test_error_code, 'http://localhost:9000', 5, '/storage/test.php', 404, "delete")
	individual_test(test_error_code, 'http://localhost:9000', 5, '/upload/upload.html', 405, "delete")
	individual_test(test_error_code, 'http://localhost:9000', 5, '/storage/unauthorise.txt', 403, "delete")
	individual_test(test_error_code, 'http://localhost:4001', 5, '/tools/magic_guesser.html', 403, "get")
	individual_test(test_error_code, 'http://localhost:3501', 5, '/cgi-bin/fail_500.py', 500, "get")
	individual_test(test_error_code, 'http://localhost:3501', 5, '/cgi-bin/color.py', 404, "get")
	individual_test(test_error_code, 'http://localhost:3501', 5, '/cgi-bin/hello.php', 405, "get")
	test_error_default('http://localhost:8080', 5)
	test_error_timeout('localhost', 35)
	test_error_max_header_size('http://localhost:9000/upload/', 5)
	test_error_max_body_size('http://localhost:9000/upload/', 5)
	test_301_redirect('http://localhost:4001', 5)
	
	print("\n===========")
	print("CONCURRENT TESTING")
	print("===========\n")
	concurrent_test(test_get_request, 200, 'http://localhost:8080', 7, None)
	concurrent_test(test_cgi_request, 200, 'http://localhost:3501', 7, '/cgi-bin/meme.py', None)
	concurrent_test(test_cgi_request, 200, 'http://localhost:3501', 15, '/cgi-bin/say_hello.py', {'name': 'username'})

	print("\n===========")
	print("SERVER_NAMES TESTING")
	print("===========\n")
	test_server_names() #Add a virtual server to test this part (tanzania)

	print("\n===========")
	print("MULTIPORT TESTING")
	print("===========\n")
	test_server_multiports()

	print("\n===========")
	print("BROWSER TESTING")
	print("===========\n")
	test_server_browsers('http://localhost:8080', 5)

	print("\n===================================")
	print("UPLOAD AND DELETE CONCURRENCE TESTING")
	print("===================================")
	num_files = 50
	print("\nTESTING CONCURRENT UPLOADS AND DELETES")
	test_upload_and_delete("http://localhost:9000/storage","http://localhost:9000/storage", num_files, 50)
	cleanup_files(num_files)

	# print("\nTESTING CONCURRENT UPLOADS")
	# upload_success, upload_total, upload_time = test_concurrent_uploads("http://localhost:9000/storage", num_files, 50)
	# print("\nTESTING CONCURRENT DELETES")
	# delete_success, delete_total, delete_time = test_concurrent_deletes("http://localhost:9000/storage", num_files, 30)
	# print("FINAL SUMMARY")
	# print(f"Upload results: {upload_success}/{upload_total} in {upload_time:.2f}s")
	# print(f"Delete results: {delete_success}/{delete_total} in {delete_time:.2f}s")
	# cleanup_files(num_files)


	print("\nTest completed!")

print(simple_test())
