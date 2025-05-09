need to include and test with ipv6 address 

request: 
GET /favicon.ico HTTP/1.1
Host: localhost:8080
Connection: keep-alive
sec-ch-ua-platform: "Linux"
User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/135.0.0.0 Safari/537.36
sec-ch-ua: "Google Chrome";v="135", "Not-A.Brand";v="8", "Chromium";v="135"
sec-ch-ua-mobile: ?0
Accept: image/avif,image/webp,image/apng,image/svg+xml,image/,/*;q=0.8
Sec-Fetch-Site: same-origin
Sec-Fetch-Mode: no-cors
Sec-Fetch-Dest: image
Referer: http://localhost:8080/
Accept-Encoding: gzip, deflate, br, zstd
Accept-Language: en-GB,en;q=0.9

# `sec-ch-*` and `Sec-Fetch-*` Headers: Not Part of HTTP/1.1 Standard

For a basic HTTP/1.1 compliant server, these headers are **not required to be processed**:

## Non-HTTP/1.1 Standard Headers in Your Request:

1. **`sec-ch-ua-platform`**: Part of User-Agent Client Hints
2. **`sec-ch-ua`**: Part of User-Agent Client Hints
3. **`sec-ch-ua-mobile`**: Part of User-Agent Client Hints
4. **`Sec-Fetch-Site`**: Part of Fetch Metadata
5. **`Sec-Fetch-Mode`**: Part of Fetch Metadata
6. **`Sec-Fetch-Dest`**: Part of Fetch Metadata

## Core HTTP/1.1 Headers (Should Handle):

1. **`Host`**: Required by HTTP/1.1
2. **`Connection`**: Important for connection management
3. **`User-Agent`**: Standard client identifier
4. **`Accept`**: Content negotiation
5. **`Referer`**: Origin of request
6. **`Accept-Encoding`**: Compression negotiation
7. **`Accept-Language`**: Language preference

The `sec-ch-*` and `Sec-Fetch-*` headers are newer additions to the HTTP ecosystem (circa 2019) designed to provide more structured client information and security context. They're not part of the original HTTP/1.1 specification (RFC 2616 or the updated RFC 7230-7235).

For your HTTP/1.1 server implementation, you can safely store these non-standard headers in your `unknown_` map without special processing and still be fully compliant with HTTP/1.1.
