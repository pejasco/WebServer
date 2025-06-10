#!/bin/bash

SCRIPT="./birthday.py"
GET_QUERY="day=13&month=11"
POST_DATA="day=13&month=11"

echo "=== Testing CGI GET Request ==="
export REQUEST_METHOD=GET
export QUERY_STRING="$GET_QUERY"
$SCRIPT

echo -e "\n\n=== Testing CGI POST Request ==="
export REQUEST_METHOD=POST
export CONTENT_TYPE="application/x-www-form-urlencoded"
export CONTENT_LENGTH=${#POST_DATA}
echo "$POST_DATA" | $SCRIPT
