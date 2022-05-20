#!/bin/bash

previousPosition=0
previousDirection=0
previousSpeed=0


while true; do
        mosquitto_pub -t "esp32/start" -h "192.168.50.117" -m "!"
        currentPosition=$(mosquitto_sub -t "esp32/position" -h "192.168.50.117" -C 1)
        currentSpeed=$((currentPosition-previousPosition))

        if [[ $currentSpeed -lt 0 ]]; then
                currentSpeed=$((-1*currentSpeed))
        fi

        if [[ $currentSpeed -lt $previousSpeed ]]; then
                currentAcceleration=0
        else
                currentAcceleration=1
        fi

        if [[ $currentPosition -lt $previousPosition ]]; then
                currentDirection=0
        else
                currentDirection=1
        fi

        printf "\033c"

        if [[ $currentAcceleration == 0 ]]; then
                echo "Slowing Down"
        fi

        if [[ $currentAcceleration == 1 ]]; then
                echo "Speeding Up"
        fi

        if [[ $currentDirection == 0 ]]; then
                echo "Moving Closer"
        fi

        if [[ $currentDirection == 1 ]]; then
                echo "Moving Farther"
        fi


        previousPosition=$currentPosition
        previousDirection=$currentDirection
        previousSpeed=$currentSpeed
done
