#!/bin/bash

Xephyr -keybd ephyr,,,xkbmodel=evdev -br -reset -screen 1400x1000x24 -dpi 96 :2 &
sleep 1

export DISPLAY=:2.0
$(dirname $0)/twindy
killall Xephyr
