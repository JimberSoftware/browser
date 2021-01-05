# Virtual microphone implementation for the browser


## Virtual mic in the docker
To create a virtual microphone please run following command:
```bash
pactl load-module module-pipe-source source_name=virtmic file=/tmp/virtmic format=s16le rate=16000 channels=1
```


## Development notes
Command to play audio into the virtmic:
```bash
ffmpeg -re -i input.mp3 -f s16le -ar 16000 -ac 1 - > /tmp/virtmic
```