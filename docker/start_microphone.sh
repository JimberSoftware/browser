#!/bin/bash

echo "start_microphone"
mkfifo /tmp/micpipe
#/usr/bin/pulseaudio --start --realtime
# should be done by start audio

pactl load-module module-pipe-source source_name=virtmic file=/tmp/virtmic format=s16le rate=41000 channels=2
ffmpeg -re -i /tmp/micpipe -f s16le -ar 41000 -ac 2 - > /tmp/virtmic