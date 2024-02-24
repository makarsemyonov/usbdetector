#!/bin/bash

make > /dev/null 2>&1

read -p "Enter the path to the file (or press Enter to use default usb.log): " filePath

if [ -z "$filePath" ]; then
    filePath="usb.log"
fi

./main $filePath > /dev/null 2>&1 &

PID=$(pgrep -f "./main")

stop_program() {
    kill $PID
    echo "Finishing..."
}


while true; do
    read -p "Enter "stop" to stop the program: " input
    if [ "$input" == "stop" ]; then
        stop_program
        break
    else
        echo "Wrong input! Try again."
    fi
done

make clean > /dev/null 2>&1

exit 0 