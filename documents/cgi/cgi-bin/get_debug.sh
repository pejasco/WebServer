#!/bin/bash

echo "Content-Type: text/html"
echo
echo "<html><body>"
echo "<h1>GET Debug</h1>"
echo "<p>Method: $REQUEST_METHOD</p>"
echo "<p>Query String: $QUERY_STRING</p>"
echo "</body></html>"