#!/usr/bin/env python3
"""
HTTP Web Server Tester
Comprehensive testing suite for HTTP web servers
"""

import requests
import json
import time
import sys
import argparse
from concurrent.futures import ThreadPoolExecutor
from urllib.parse import urljoin

class WebServerTester:
    def __init__(self, base_url, timeout=10, config_file=None):
        self.base_url = base_url.rstrip('/')
        self.timeout = timeout
        self.config_file = config_file
        self.results = []
        
    def log_result(self, test_name, success, details=""):
        """Log test result"""
        status = "PASS" if success else "FAIL"
        print(f"[{status}] {test_name}: {details}")
        self.results.append({
            'test': test_name,
            'success': success,
            'details': details
        })
    
    def test_basic_connectivity(self):
        """Test basic server connectivity"""
        try:
            response = requests.get(self.base_url, timeout=self.timeout)
            self.log_result("Basic Connectivity", True, f"Status: {response.status_code}")
            return True
        except Exception as e:
            self.log_result("Basic Connectivity", False, str(e))
            return False
    
    def test_http_methods(self):
        """Test various HTTP methods"""
        methods = ['GET', 'POST', 'DELETE']
        
        for method in methods:
            try:
                response = requests.request(method, self.base_url, timeout=self.timeout)
                self.log_result(f"Method {method}", True, f"Status: {response.status_code}")
            except Exception as e:
                self.log_result(f"Method {method}", False, str(e))
    
    def test_status_codes(self):
        """Test various endpoints for expected status codes"""
        test_cases = [
            ('/', 'Root endpoint'),
            ('/nonexistent', 'Non-existent endpoint (should be 404)'),
            ('/index.html', 'Static file'),
            ('/favicon.ico', 'Favicon'),
        ]
        
        for endpoint, description in test_cases:
            try:
                url = urljoin(self.base_url, endpoint)
                response = requests.get(url, timeout=self.timeout)
                self.log_result(f"Status Code Test - {description}", 
                              True, f"Status: {response.status_code}")
            except Exception as e:
                self.log_result(f"Status Code Test - {description}", False, str(e))
    
    def test_headers(self):
        """Test HTTP headers"""
        try:
            response = requests.get(self.base_url, timeout=self.timeout)
            headers = response.headers
            
            # Check for common headers
            expected_headers = ['Content-Type', 'Content-Length', 'Date', 'Server']
            for header in expected_headers:
                if header in headers:
                    self.log_result(f"Header {header}", True, f"Value: {headers[header]}")
                else:
                    self.log_result(f"Header {header}", False, "Missing")
                    
        except Exception as e:
            self.log_result("Headers Test", False, str(e))
    
    def test_content_types(self):
        """Test different content types"""
        test_files = [
            ('/index.html', 'text/html'),
            ('/style.css', 'text/css'),
            ('/script.js', 'text/javascript'),
            ('/image.jpg', 'image/jpeg'),
            ('/data.json', 'application/json'),
        ]
        
        for endpoint, expected_type in test_files:
            try:
                url = urljoin(self.base_url, endpoint)
                response = requests.get(url, timeout=self.timeout)
                content_type = response.headers.get('Content-Type', '').split(';')[0]
                
                if expected_type in content_type:
                    self.log_result(f"Content-Type {endpoint}", True, 
                                  f"Expected: {expected_type}, Got: {content_type}")
                else:
                    self.log_result(f"Content-Type {endpoint}", False, 
                                  f"Expected: {expected_type}, Got: {content_type}")
            except Exception as e:
                self.log_result(f"Content-Type {endpoint}", False, str(e))
    
    def test_post_data(self):
        """Test POST requests with different data types"""
        test_data = [
            ('application/x-www-form-urlencoded', {'key': 'value', 'test': 'data'}),
            ('application/json', {'message': 'Hello, Server!', 'timestamp': time.time()}),
            ('text/plain', 'This is plain text data'),
        ]
        
        for content_type, data in test_data:
            try:
                headers = {'Content-Type': content_type}
                if content_type == 'application/json':
                    response = requests.post(self.base_url, json=data, 
                                           headers=headers, timeout=self.timeout)
                elif content_type == 'application/x-www-form-urlencoded':
                    response = requests.post(self.base_url, data=data, 
                                           headers=headers, timeout=self.timeout)
                else:
                    response = requests.post(self.base_url, data=data, 
                                           headers=headers, timeout=self.timeout)
                
                self.log_result(f"POST {content_type}", True, f"Status: {response.status_code}")
            except Exception as e:
                self.log_result(f"POST {content_type}", False, str(e))
    
    def test_large_request(self):
        """Test handling of large requests"""
        try:
            large_data = 'x' * (1024 * 100)  # 100KB of data
            response = requests.post(self.base_url, data=large_data, timeout=self.timeout)
            self.log_result("Large Request", True, f"Status: {response.status_code}")
        except Exception as e:
            self.log_result("Large Request", False, str(e))
    
    def test_concurrent_requests(self, num_threads=10, num_requests=50):
        """Test concurrent request handling"""
        def make_request():
            try:
                response = requests.get(self.base_url, timeout=self.timeout)
                return response.status_code == 200
            except:
                return False
        
        start_time = time.time()
        with ThreadPoolExecutor(max_workers=num_threads) as executor:
            futures = [executor.submit(make_request) for _ in range(num_requests)]
            results = [future.result() for future in futures]
        
        end_time = time.time()
        success_rate = sum(results) / len(results) * 100
        duration = end_time - start_time
        
        self.log_result("Concurrent Requests", success_rate > 95, 
                       f"Success rate: {success_rate:.1f}%, Duration: {duration:.2f}s")
    
    def test_webserv_specific(self):
        """Test webserv-specific requirements"""
        # Test required HTTP methods
        required_methods = ['GET', 'POST', 'DELETE']
        for method in required_methods:
            try:
                response = requests.request(method, self.base_url, timeout=self.timeout)
                self.log_result(f"Required Method {method}", True, f"Status: {response.status_code}")
            except Exception as e:
                self.log_result(f"Required Method {method}", False, str(e))
        
        # Test file upload capability (POST)
        try:
            files = {'file': ('test.txt', 'This is test content', 'text/plain')}
            response = requests.post(f"{self.base_url}/upload", files=files, timeout=self.timeout)
            self.log_result("File Upload Test", True, f"Status: {response.status_code}")
        except Exception as e:
            self.log_result("File Upload Test", False, str(e))
        
        # Test static file serving
        static_files = ['/index.html', '/style.css', '/script.js', '/favicon.ico']
        for file_path in static_files:
            try:
                url = f"{self.base_url}{file_path}"
                response = requests.get(url, timeout=self.timeout)
                self.log_result(f"Static File {file_path}", True, f"Status: {response.status_code}")
            except Exception as e:
                self.log_result(f"Static File {file_path}", False, str(e))
        
        # Test directory listing (if enabled)
        try:
            response = requests.get(f"{self.base_url}/uploads/", timeout=self.timeout)
            if "Index of" in response.text or response.status_code == 403:
                self.log_result("Directory Listing", True, f"Status: {response.status_code}")
            else:
                self.log_result("Directory Listing", False, "No directory listing found")
        except Exception as e:
            self.log_result("Directory Listing", False, str(e))
    
    def test_cgi_support(self):
        """Test CGI support (PHP, Python, etc.)"""
        cgi_tests = [
            ('/cgi-bin/test.php', 'PHP CGI'),
            ('/cgi-bin/test.py', 'Python CGI'),
            ('/test.php', 'Direct PHP'),
        ]
        
        for endpoint, description in cgi_tests:
            try:
                url = f"{self.base_url}{endpoint}"
                response = requests.get(url, timeout=self.timeout)
                if response.status_code == 200:
                    self.log_result(f"CGI Test - {description}", True, 
                                  f"Status: {response.status_code}, Content-Type: {response.headers.get('Content-Type', 'N/A')}")
                else:
                    self.log_result(f"CGI Test - {description}", False, f"Status: {response.status_code}")
            except Exception as e:
                self.log_result(f"CGI Test - {description}", False, str(e))
    
    def test_error_pages(self):
        """Test custom error pages"""
        error_codes = [404, 403, 500, 501]
        
        for code in error_codes:
            try:
                if code == 404:
                    url = f"{self.base_url}/nonexistent_page"
                elif code == 403:
                    url = f"{self.base_url}/forbidden"
                else:
                    url = f"{self.base_url}/error{code}"
                
                response = requests.get(url, timeout=self.timeout)
                self.log_result(f"Error Page {code}", True, 
                               f"Status: {response.status_code}, Has custom content: {'<html>' in response.text.lower()}")
            except Exception as e:
                self.log_result(f"Error Page {code}", False, str(e))
    
    def test_request_body_limits(self):
        """Test maximum request body size limits"""
        # Test small request (should work)
        try:
            small_data = 'x' * 1024  # 1KB
            response = requests.post(self.base_url, data=small_data, timeout=self.timeout)
            self.log_result("Small Request Body", True, f"Status: {response.status_code}")
        except Exception as e:
            self.log_result("Small Request Body", False, str(e))
        
        # Test large request (might be rejected)
        try:
            large_data = 'x' * (1024 * 1024 * 10)  # 10MB
            response = requests.post(self.base_url, data=large_data, timeout=self.timeout)
            if response.status_code == 413:  # Request Entity Too Large
                self.log_result("Large Request Body Limit", True, "Correctly rejected large request (413)")
            else:
                self.log_result("Large Request Body Limit", True, f"Accepted large request (Status: {response.status_code})")
        except Exception as e:
            self.log_result("Large Request Body Limit", False, str(e))
    
    def test_http_redirects(self):
        """Test HTTP redirects"""
        redirect_tests = [
            ('/redirect', 'Basic redirect'),
            ('/old-page', 'Legacy page redirect'),
            ('/', 'Root redirect (if configured)'),
        ]
        
        for endpoint, description in redirect_tests:
            try:
                url = f"{self.base_url}{endpoint}"
                response = requests.get(url, allow_redirects=False, timeout=self.timeout)
                if response.status_code in [301, 302, 303, 307, 308]:
                    location = response.headers.get('Location', 'No Location header')
                    self.log_result(f"Redirect Test - {description}", True, 
                                  f"Status: {response.status_code}, Location: {location}")
                else:
                    self.log_result(f"Redirect Test - {description}", False, 
                                  f"No redirect found (Status: {response.status_code})")
            except Exception as e:
                self.log_result(f"Redirect Test - {description}", False, str(e))
    
    def test_multiple_servers(self):
        """Test multiple server configurations (different ports/hosts)"""
        # This would need to be configured based on your webserv config
        test_ports = [8080, 8081, 8082]
        base_host = self.base_url.split('://')[1].split(':')[0]
        
        for port in test_ports:
            try:
                test_url = f"http://{base_host}:{port}"
                response = requests.get(test_url, timeout=5)
                self.log_result(f"Multiple Server Port {port}", True, f"Status: {response.status_code}")
            except Exception as e:
                self.log_result(f"Multiple Server Port {port}", False, str(e))
    
    def test_chunked_encoding(self):
        """Test chunked transfer encoding"""
        headers = {'Transfer-Encoding': 'chunked'}
        data = "5\r\nHello\r\n5\r\nWorld\r\n0\r\n\r\n"
        response = requests.post(self.base_url, data=data, headers=headers, timeout=self.timeout)
        self.log_result("Chunked Encoding", True, f"Status: {response.status_code}")
    def test_keep_alive(self):
        """Test HTTP keep-alive connections"""
        try:
            session = requests.Session()
            for i in range(5):
                response = session.get(self.base_url, timeout=self.timeout)
                if response.status_code != 200:
                    self.log_result("Keep-Alive", False, f"Request {i+1} failed")
                    return
            
            self.log_result("Keep-Alive", True, "Multiple requests successful")
        except Exception as e:
            self.log_result("Keep-Alive", False, str(e))
    
    def run_all_tests(self):
        """Run all tests"""
        print(f"Starting webserv tests for {self.base_url}")
        print("=" * 60)
        
        # Basic connectivity first
        if not self.test_basic_connectivity():
            print("Server not reachable, stopping tests.")
            return
        
        # Run core HTTP tests
        self.test_http_methods()
        self.test_status_codes()
        self.test_headers()
        self.test_content_types()
        
        # Run webserv-specific tests
        print("\n" + "-" * 40)
        print("WEBSERV SPECIFIC TESTS")
        print("-" * 40)
        self.test_webserv_specific()
        self.test_cgi_support()
        self.test_error_pages()
        self.test_request_body_limits()
        self.test_http_redirects()
        self.test_multiple_servers()
        self.test_chunked_encoding()
        
        # Run performance tests
        print("\n" + "-" * 40)
        print("PERFORMANCE TESTS")
        print("-" * 40)
        self.test_post_data()
        self.test_large_request()
        self.test_keep_alive()
        self.test_concurrent_requests()
        
        # Summary
        print("\n" + "=" * 60)
        passed = sum(1 for r in self.results if r['success'])
        total = len(self.results)
        print(f"Tests completed: {passed}/{total} passed ({passed/total*100:.1f}%)")
        
        # Show failed tests
        failed_tests = [r for r in self.results if not r['success']]
        if failed_tests:
            print(f"\nFailed tests ({len(failed_tests)}):")
            for test in failed_tests:
                print(f"  - {test['test']}: {test['details']}")

def main():
    parser = argparse.ArgumentParser(description='Webserv HTTP Server Tester')
    parser.add_argument('url', help='Base URL of the webserv server to test')
    parser.add_argument('--config', help='Path to webserv configuration file')
    parser.add_argument('--timeout', type=int, default=10, help='Request timeout in seconds')
    parser.add_argument('--threads', type=int, default=10, help='Number of concurrent threads for load test')
    parser.add_argument('--requests', type=int, default=50, help='Number of requests for load test')
    
    args = parser.parse_args()
    
    tester = WebServerTester(args.url, args.timeout, args.config)
    tester.run_all_tests()

if __name__ == '__main__':
    main()