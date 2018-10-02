#!/bin/bash
# REQUIRES g++ (for ptp2bin), Java JRE (for AppleCommander), and curl

# Build ptp2bin.
echo "Building ptp2bin."
g++ ptp2bin.cpp -o ptp2bin

# Convert logo.ptp
echo "Converting PTP."
START=1FFA
LENGTH=7605
BIN=aplogo.a2bin
./ptp2bin $START $LENGTH logo.ptp $BIN

# Get AppleCommander
echo "Getting AppleCommander."
AC=AppleCommander.jar
if [ ! -f $AC ]; then
	curl -L https://github.com/AppleCommander/AppleCommander/releases/download/v1-4-0/AppleCommander-ac-1.4.0.jar -o $AC
fi

echo "Building disk image."
ACRUN="java -XstartOnFirstThread -jar $AC"
#$ACRUN -dos140 LOGO.dsk
$ACRUN -d LOGO.dsk LOGO
cat $BIN | $ACRUN -p LOGO.dsk LOGO B 0x$START
