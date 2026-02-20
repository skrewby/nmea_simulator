#!/usr/bin/env bash

helpFunction() {
  echo ""
  echo "Usage: $0 <command>"
  echo -e "\tcommand:   Tells the script what to do. Options:"
  echo -e "\t\tbuild:   Builds and compiles the example"
  echo -e "\t\tsetup:   Setups up vcan0 interface"
  echo -e "\t\trun:     Runs the program"
  echo -e "\t\thelp:    Shows this prompt"
}

buildProgram() {
    cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build
    cmake --build build
}

if [[ $1 == "help" || -z $1 ]]; then
    helpFunction
elif [[ $1 == "build" ]]; then
    buildProgram
elif [[ $1 == "setup" ]]; then
    sudo modprobe vcan
    sudo ip link add dev vcan0 type vcan
    sudo ip link set up vcan0
elif [[ $1 == "run" ]]; then
    buildProgram && ./bin/nmea_simulator -c vcan0
fi
