#!/bin/bash

echo "Proxy Integration Test"

echo "Start Proxy Server"
sudo ../build/bin/server ../config/proxy_config &

sleep 1

echo "Fetch file from proxy"
curl -i http://localhost/ucla/contact > ./proxy.txt

echo "Fetch file from source"
curl -i http://www.ucla.edu/contact > ./source.txt

DIFF=$(diff -w -B ./proxy.txt ./source.txt)
if [ $? -eq 0 ]
then
    echo "PASSED Echo handler integration test"
else
    echo "FAILED Echo handler integration test"
    echo $DIFF
fi 

echo "Shutting down Server"
pkill -f main
#rm ./ucla.txt
#rm ./source.txt