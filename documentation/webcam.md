# Virtual webcam implementation for the browser

This project uses Video4Linux2 (v4l2):  
[https://github.com/umlaeute/v4l2loopback](https://github.com/umlaeute/v4l2loopback)
## Virtcams on the server
To enable to virtual cam's on the server you must run following command:
```bash
sudo modprobe v4l2loopback devices=7 exclusive_caps=1,1,1,1,1,1,1
```

To create more than 8 virtual devices you should compile v4l2 yourself.  
Use `make KCPPFLAGS="-DMAX_DEVICES=100"`  
https://github.com/umlaeute/v4l2loopback/blob/master/v4l2loopback.c#L156

## Development notes
Command to play a video into the virtcam:
```bash
ffmpeg -re -i video.webm -f v4l2 /dev/video0
```