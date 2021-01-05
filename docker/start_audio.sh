#!/bin/bash

echo "start_audio.sh!"
#/usr/bin/pulseaudio --start --realtime
/usr/bin/pulseaudio --start
#pacmd list-source-outputs
# ./start_microphone.sh &

until exec 3<>/dev/tcp/localhost/6663
do
    echo "nope"
    sleep 1
done
echo "Audio socket connected!"
# pacat --record -d auto_null.monitor --latency-msec=1 | oggenc -b 192 --raw - 1>&3
# pacat --record -d auto_null.monitor --latency-msec=200 | lame -r - - >&3
# pacat  --record --format s16le --channels 1 -d auto_null.monitor --rate 44100 --latency-msec=1 >&3 CPU usage too high
pacat  --record --format s16le --channels 1 -d auto_null.monitor --rate 44100 --latency-msec=10 >&3



# /usr/bin/pulseaudio --start --realtime
# pactl load-module module-pipe-source source_name=virtmic file=/tmp/virtmic format=s16le rate=16000 channels=1

# ffmpeg -re -i /tmp/micpipe -f s16le -ar 16000 -ac 1 - > /tmp/virtmic