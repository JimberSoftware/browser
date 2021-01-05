#!/bin/bash

echo "start_audio.sh!"
/usr/bin/pulseaudio --start --realtime
#pacmd list-source-outputs
exec 3<>/dev/tcp/192.168.2.52/6663
pacat --record -d auto_null.monitor --latency-msec=1 | oggenc -b 192 --raw - 1>&3

# pacat --record -d auto_null.monitor --latency-msec=1 | ffmpeg -f u16le -ar 44100 -ac 1 -i -
# pacat --record -d auto_null.monitor --latency-msec=1 | ffmpeg -f s16le -ar 44100 -ac 1 -i -
# pacat --record -d auto_null.monitor --latency-msec=1 | ffmpeg -f s16le -ar 44100 -ac 1 -i -
# pacat --record -d auto_null.monitor --latency-msec=1 | lame - >&3
pacat --record -d auto_null.monitor --latency-msec=1 | lame -r - >&3

exec 3<>/dev/tcp/localhost/6663
pacat --record --format u8 -d auto_null.monitor >&3
pacat --record --format u8 -d auto_null.monitor | ffmpeg -f u8 -ar 44.1k -ac 1 -i - -f u8 tcp://localhost:6663
# pacat --record --format u16le -d auto_null.monitor | ffmpeg -f u16le -ar 44.1k -ac 1 -i - -f u16le tcp://localhost:6663

pacat --record --format s16be --channels 1 -d auto_null.monitor --rate 44100 | ffmpeg -f s16be -ar 44100 -ac 1 -i - -f s16be tcp://localhost:6663

/usr/bin/pulseaudio --start --realtime
pacat --record --format s16le --channels 1 -d auto_null.monitor --rate 44100  --latency-msec=200 | ffmpeg -f s16le -ar 44100 -ac 1 -i - -f s16le tcp://localhost:6663

#works with button
pacat --record --format s16le --channels 1 -d auto_null.monitor --rate 44100  --latency-msec=200 | ffmpeg -f s16le -ar 44100 -ac 1 -i - -f s16le tcp://localhost:6663
pacat --record --format s16le --channels 1 -d auto_null.monitor --rate 44100  --latency-msec=100 | ffmpeg -f s16le -ar 44100 -ac 1 -i - -f s16le tcp://localhost:6663
pacat --record --format s16le --channels 1 -d auto_null.monitor --rate 44100  --latency-msec=100 | ffmpeg -f s16le -ar 44100 -ac 1 -i - -f s16le test.pcm


# pacat --record --format s16le --channels 1 -d auto_null.monitor --rate 8000 --latency-msec=100 | ffmpeg -f s16be -ar 8000 -ac 1 -i - -f s16le tcp://localhost:6663

exec 3<>/dev/tcp/localhost/6663
pacat  --record --format s16le --channels 1 -d auto_null.monitor --rate 44100  --latency-msec=100 | ./pcmtomp3 >&3