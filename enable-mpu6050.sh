#!/bin/bash

# gpio(1) 28 and 29 for power - 1*32+28 = 50
echo 50 > /sys/class/gpio/export
echo 51 > /sys/class/gpio/export
# gpio(0) 14 for interrupt - 0*32+14 = 14
echo 14 > /sys/class/gpio/export

echo 'out' > /sys/class/gpio/gpio50/direction
echo 'out' > /sys/class/gpio/gpio51/direction
echo 'in' > /sys/class/gpio/gpio14/direction

# create interrupts on falling edge of gpio14
echo 'falling' > /sys/class/gpio/gpio14/edge

echo 0 > /sys/class/gpio/gpio51/value
echo 1 > /sys/class/gpio/gpio50/value

