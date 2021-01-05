FROM qtcodebuilder:5.15 as builder

ARG PRIVATE_TOKEN_GITLAB
RUN curl --location --output artifacts.zip --header "PRIVATE-TOKEN: $PRIVATE_TOKEN_GITLAB" https://labs.jimber.org/api/v4/projects/21/jobs/artifacts/5.15.1/download?job=build:qtwebengine ;
RUN unzip artifacts.zip
RUN ls libexec

FROM ubuntu:20.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt install -y wget python libnss3 libegl1-mesa-dev libfontconfig libxcomposite1 \
    libxcursor1 libxdamage1 \
    libxext6 libxfixes3 \
    libxi6 libxrandr2 \
    libxrender1 libxss1 \
    libxtst6 libasound2 \
    libdbus-1-3 libglib2.0-0 \
    libavformat58 libswscale5 \
    iputils-ping alsa libasound2 pulseaudio \
    v4l2loopback-dkms ffmpeg libpulse-mainloop-glib0 \
    libevent-dev libwebp-dev libminizip-dev libwebpdemux2
RUN apt-get update -y && echo ttf-mscorefonts-installer msttcorefonts/accepted-mscorefonts-eula select true | debconf-set-selections && apt-get install -y ttf-mscorefonts-installer
ENV QTDIR=/opt/Qt/5.15.1/gcc_64
ENV PATH="$QTDIR/bin:$PATH"
ENV LD_LIBRARY_PATH="$QTDIR/lib:$LD_LIBRARY_PATH"
RUN mkdir -p /opt/jimber
RUN mkdir /tmp/uploads
WORKDIR /opt/jimber
RUN echo "Good job, please show us how you did this at info@jimber.org" > /flag.txt
COPY --from=builder /opt/Qt/5.15.1/gcc_64 /opt/Qt/5.15.1/gcc_64
# COPY --from=builder /opt/Qt/5.15.1/Src /opt/Qt/5.15.1/Src

COPY --from=builder lib /opt/Qt/5.15.1/gcc_64/lib
# COPY --from=builder /home/user/libexec/QtWebEngineProcess /opt/jimber/QtWebEngineProcess
COPY ./bins /opt/jimber
COPY ./libs /opt/Qt/5.15.1/gcc_64/plugins/platforms
COPY widevine.sh widevine.sh
RUN chmod +x widevine.sh
RUN ./widevine.sh
COPY start_webcam.sh start_webcam.sh
COPY start_audio.sh start_audio.sh
COPY start_microphone.sh start_microphone.sh
COPY start.sh start.sh
COPY image.webm image.webm
COPY input.mp3 input.mp3
RUN chmod +x start.sh start_audio.sh start_webcam.sh start_microphone.sh
# CMD ["/opt/jimber/browser", "--platform", "jimber", "--no-sandbox", "--disable-dev-shm-usage"]
CMD ["./start.sh"]