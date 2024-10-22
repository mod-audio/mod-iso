#!/bin/bash

if [ "$TWINDY_SKIP_LOCAL_TEST"x != "1"x ]; then
  export TWINDY_SKIP_JACKD_START=1
  export TWINDY_LOCAL_TEST=1
fi

Xephyr -keybd ephyr,,,xkbmodel=evdev -br -reset -screen 1920x979x24 -resizeable -dpi 96 :2 &
sleep 1

export DISPLAY=:2.0
gdb -ex run $(dirname $0)/twindy-dbg
# valgrind --leak-check=full $(dirname $0)/twindy-dbg
killall mod-host Xephyr
