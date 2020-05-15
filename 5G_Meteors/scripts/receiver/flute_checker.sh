PROCESS="flute"
PROCANDARGS="./flute -A -U -B:DASH_Content1 -m:192.168.3.30 -s:192.168.2.20 -p:4001 -t:1 -Y:1 -Q"

while :
do
RESULT=`pgrep -x ${PROCESS}`
    if [ "${RESULT:-null}" = null ]; then
        echo "${PROCESS} not running, starting "$PROCANDARGS
        $PROCANDARGS &
    fi
    sleep 2
done
