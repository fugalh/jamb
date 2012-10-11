#!/bin/bash
SRC=20:0
DST=$(aconnect -o | grep "'aeolus'" | grep -o '[0-9]*'):0
aconnect $SRC $DST
