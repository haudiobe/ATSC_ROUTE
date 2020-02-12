#!/bin/sh

PROCESS="flute"
PROCANDARGS="./flute -A -U -B:Dash_Content_5Mbps_SDP -m:10.0.2.15 -s:10.0.2.15 -p:4001 -t:1 -Y:1 -x:2 -X:1 -Q"

while :
do
    RESULT=`pgrep -x ${PROCESS}`

    if [ "${RESULT:-null}" = null ]; then
            echo "${PROCESS} not running, starting "$PROCANDARGS
            $PROCANDARGS &
    else
            echo "running"
    fi
    sleep 10
done 
