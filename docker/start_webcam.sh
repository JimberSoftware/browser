#!/bin/bash

echo "Start webcam ffmpeg"
mkfifo /tmp/campipe
ffmpeg -i /tmp/campipe -f v4l2 /dev/video0 -loglevel panic
echo "Webcam stream has exited"