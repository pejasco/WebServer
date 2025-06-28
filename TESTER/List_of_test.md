## 🔴 MANDATORY HTTP REQUEST & RESPONSE

### Basic Server Functionality
- [x] **Server starts with configuration file argument** ✅ MANDATORY
- [x] **Server uses default path when no config provided** ✅ MANDATORY
- [x] **Multiple servers with different host:port combinations** ✅ MANDATORY
- [x] **Default server selection for host:port** ✅ MANDATORY
- [x] **Server names setup (optional in config, but must handle when present)** ✅ MANDATORY

### HTTP Methods (All Required)
- [x] **GET requests** ✅ MANDATORY
  - Static files (HTML, CSS, JS, images)
  - Directory requests
  - Non-existent files (404)
  - Root directory access
- [x] **POST requests** ✅ MANDATORY
  - Form data submission
  - File uploads
  - Request body size limits
- [x] **DELETE requests** ✅ MANDATORY
  - Delete existing files
  - Delete non-existent files

### Error Handling & Status Codes
- [x] **Accurate HTTP response status codes** ✅ MANDATORY
- [x] **Default error pages when none provided** ✅ MANDATORY
- [x] 200 OK - Successful requests ✅ MANDATORY
- [x] 400 Bad Request - Malformed requests ✅ MANDATORY
- [x] 404 Not Found - Missing resources ✅ MANDATORY
- [x] 405 Method Not Allowed - Unsupported methods ✅ MANDATORY
- [x] 500 Internal Server Error - Server errors ✅ MANDATORY

## Static Website Serving
- [x] **Serve fully static website** ✅ MANDATORY
- [x] **File serving (HTML, CSS, JS, images)** ✅ MANDATORY
- [x] **Large file serving** ✅ MANDATORY
- [x] **Binary file serving** ✅ MANDATORY

### CGI (Common Gateway Interface)
- [x] **At least one CGI support (PHP, Python, etc.)** ✅ MANDATORY
- [x] **CGI with GET method** ✅ MANDATORY
- [x] **CGI with POST method** ✅ MANDATORY
- [x] **CGI working directory (correct directory for relative paths)** ✅ MANDATORY
- [x] **CGI called with file as first argument** ✅ MANDATORY
- [x] **PATH_INFO as full path** ✅ MANDATORY

### File Upload Tests
- [x] **Clients must be able to upload files** ✅ MANDATORY
- [x] **File upload via POST** ✅ MANDATORY

### Browser Compatibility
- [x] **Compatible with standard web browsers** ✅ MANDATORY
- [x] **HTTP/1.1 compliance (compare with NGINX)** ✅ MANDATORY

### 🟡 NICE TO HAVE
- [x] Custom error pages (when configured) 🟡 OPTIONAL
- [x] HTTP redirects (when configured) 🟡 OPTIONAL
- [x] 403 Forbidden 🟡 DEPENDS ON IMPLEMENTATION
- [x] 413 Payload Too Large 🟡 DEPENDS ON CONFIG
- [x] Directory listing (when enabled in config) 🟡 OPTIONAL
- [x] Default index files (when configured) 🟡 OPTIONAL
- [x] Multiple file upload 🟡 NICE TO HAVE
- [x] Large file upload limits 🟡 DEPENDS ON CONFIG
- [x] HTTP redirect definition 🟡 OPTIONAL (if configured)
- [x] Directory listing enable/disable 🟡 OPTIONAL (if configured)
- [x] Default file for directory requests 🟡 OPTIONAL (if configured)
- [x] Maximum client request body size 🟡 OPTIONAL (if configured)
- [x] CGI execution by file extension 🟡 OPTIONAL (if configured)
- [x] Upload file location configuration 🟡 OPTIONAL (if configured)

### Security & Edge Cases (Bonus)
- [x] Extremely long URLs/headers 🟡 ROBUSTNESS BONUS
- [x] Binary data in requests 🟡 ROBUSTNESS BONUS

### Advanced Protocol Features (Nice to Have)
- [x] Keep-alive connections 🟡 NICE TO HAVE
- [x] Complex header handling 🟡 NICE TO HAVE
- [x] Query string parsing 🟡 NICE TO HAVE

## 🔴 MANDATORY PERFORMANCE & RELIABILITY

### Non-blocking Behavior (CRITICAL)
- [x] **Server must remain non-blocking at all times** ✅ MANDATORY
- [x] **Only 1 poll() (or equivalent) for all I/O operations** ✅ MANDATORY
- [x] **Never read/write without poll()** ✅ MANDATORY
- [x] **Monitor reading and writing simultaneously** ✅ MANDATORY
- [x] **Proper client disconnection handling** ✅ MANDATORY
- [x] **No hanging requests (never hang indefinitely)** ✅ MANDATORY

### Multi-port Support
- [x] **Listen to multiple ports** ✅ MANDATORY

### 🟡 NICE TO HAVE Stress Testing (Good Practice)
- [x] Multiple simultaneous connections 🟡 GOOD PRACTICE
- [x] Server uptime under load 🟡 GOOD PRACTICE
- [ ] Memory leak detection 🟡 GOOD PRACTICE
- [x] Resource cleanup verification 🟡 GOOD PRACTICE

## 🔴 MANDATORY CONFIGURATION FEATURES

### Configuration File
- [] ***Empty configuration file handling***
- [] ***Missing minimum information in configuration file***
    - 1 server block
    - 1 host
    - 1 port
    - 1 location block
    - 1 methods
    - 1 root
    - 1 index  
- [] ***Double information (same ports, same ip on the same server block, etc...)***

## ❌ EXPLICITLY FORBIDDEN / NOT REQUIRED

### What You DON'T Need to Implement
- ❌ **Virtual host feature** (explicitly out of scope)
- ❌ **Complete RFC implementation** (only subset required)
- ❌ **Using execve for web server** (forbidden)
- ❌ **Using fork except for CGI** (forbidden)
- ❌ **RegExp for routes** (routes won't use regexp)
- ❌ **Custom configuration syntax** (can inspire from NGINX)
