#!/bin/bash

if [ "$TWINDY_SKIP_LOCAL_TEST"x != "1"x ]; then
  export TWINDY_LOCAL_TEST=1
fi

Xephyr -keybd ephyr,,,xkbmodel=evdev -br -reset -screen 1280x979x24 -resizeable -dpi 96 :2 &
sleep 1

export DISPLAY=:2.0
$(dirname $0)/twindy
killall jackd ingen Xephyr
