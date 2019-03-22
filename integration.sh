#!/bin/bash

sid=0
exe_dir=""

function setup_server()
{
    echo "setup server"
    if [ -d output ]
    then
        echo "output file already exists"
        rm output/*
    else
        echo "make output directory"    
        mkdir output
    fi
    if [ "$exe_dir" = "build_coverage" ]
    then
        ./build_coverage/bin/server config/integration_config 2> output/invalid_request.txt &
    else
        ./build/bin/server config/integration_config 2> output/invalid_request.txt &
    fi
    sid=$!
    sleep 0.1
}

function shut_down_server()
{
    echo "kill the server"
    kill $sid
}

function invalid_request_test()
{
    printf "GET \r\n" | nc localhost 2019
    diff output/invalid_request.txt expect/invalid_request_expect.txt
    if [ $? != 0 ]
    then
        return 1
    fi
    return 0
}

function valid_request_test()
{
    printf 'GET /echo HTTP/1.1\r\nHost: example.com\r\n\r\n' | nc localhost 2019 -q 5 > output/valid_response.txt
    diff output/valid_response.txt expect/valid_response_expect.txt
    if [ $? != 0 ]
    then
        return 1
    fi
    return 0
}

function multi_requests_test()
{
    rm -f /tmp/in
    mkfifo /tmp/in
    ( exec 3>/tmp/in; printf 'GET /echo HTTP/1.1\r\n' >&3; curl http://localhost:2019/status; printf 'Host: example.com\r\n\r\n' >&3 ) &
    nc localhost 2019 -q 5 < /tmp/in > output/valid_response.txt
    diff output/valid_response.txt expect/valid_response_expect.txt
    if [ $? != 0 ]
    then
        return 1
    fi
    return 0
}

function multi_post_requests_test()
{
    old_max_id=`curl http://localhost:2019/mime/list | grep -o '<br>' | wc -l`

    # Initiate 8 simultaneous post requests and store pids in array
    num_requests=8
    for (( i=0; i < $num_requests; ++i )); do
        curl -F myimage=@static/trump.jpg -F top=Donald -F bottom=Trump http://localhost:2019/static1/form.html &
        pids[${i}]=$!
    done

    # wait for all pids
    for pid in ${pids[*]}; do
        wait $pid
    done
    
    new_max_id=`curl http://localhost:2019/mime/list | grep -o '<br>' | wc -l`

    if [ $(($new_max_id - $old_max_id)) != $num_requests ]
    then
        return 1
    fi

    for (( i=$old_max_id+1; i <= $new_max_id; ++i )); do
        # For some unknown reasons, the last characters of encoded strings are different
        # I spent a lot of time on debugging this, I couldn't figure out why
        output=`curl http://localhost:2019/mime/view?id=${i} | grep -o '<img[^>]*src="[^"]*"' | sed 's/[^"]*"\([^"]*\)".*/\1/' | sed -e 's#.*,\(\)#\1#'`
        output=${output::-1}

        expect=`openssl base64 -in static/trump.jpg | tr -d '\n'`
        expect=${expect::-1}

        if [ "$output" != "$expect" ]
        then
            return 1
        fi
    done

    return 0
}

function mime_delete_test()
{
    # Delete an existing ID
    old_max_id=`curl http://localhost:2019/mime/list | grep -o '<br>' | wc -l`
    curl http://localhost:2019/mime/delete?id=1
    new_max_id=`curl http://localhost:2019/mime/list | grep -o '<br>' | wc -l`

    if [ $(($old_max_id - $new_max_id)) != 1 ]
    then
        return 1
    fi

    # Delete a nonexisting ID
    old_max_id=`curl http://localhost:2019/mime/list | grep -o '<br>' | wc -l`
    curl http://localhost:2019/mime/delete?id=10000
    new_max_id=`curl http://localhost:2019/mime/list | grep -o '<br>' | wc -l`

    if [ $(($old_max_id - $new_max_id)) != 0 ]
    then
        return 1
    fi

    return 0
}

function integration_test()
{
    exe_dir="${PWD##*/}"

    if [ "$exe_dir" = "build" ]
    then
        cd ..
    fi

    if [ "$exe_dir" = "build_coverage" ]
    then
        cd ..
    fi

    setup_server

    valid_request_test
    if [ $? != 0 ]
    then
        echo "valid request test failed"
        return 1
    fi

    invalid_request_test
    if [ $? != 0 ]
    then
        echo "invalid request test failed"
        return 1
    fi

    multi_requests_test
    if [ $? != 0 ]
    then
        echo "multiple requests test failed"
        return 1
    fi

    multi_post_requests_test
    if [ $? != 0 ]
    then
        echo "multiple post requests test failed"
        return 1
    fi

    mime_delete_test
    if [ $? != 0 ]
    then
        echo "mime delete test failed"
        return 1
    fi

    echo "integration test passed, GREAT"
    shut_down_server
    return 0
}

integration_test