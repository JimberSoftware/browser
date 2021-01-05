# Jimber browser
## General setup

Download and install Qt 5.13.2

### Install general dependencies

Tested for ubuntu 20.04 or ubuntu based distros
```bash
sudo apt-get install \
libgl-dev \
libavformat-dev \
libswscale-dev \
libfontconfig-dev \
libfreetype6-dev \
```

Add the Qt binaries to the path variable and add the libs to the `LD_LIBRARY_PATH`

```
export QTDIR="<QT_path>/5.13.2/gcc_64"
export PATH="$QTDIR/bin:$PATH"
export LD_LIBRARY_PATH="$QTDIR/lib:$LD_LIBRARY_PATH"
```



Jimber has changed some of the default methods included in the Qt source base. Replace both in your install location **Qt/5.13.2/Src/qtwebengine/src/webenginewidgets/api**

* qwebengineview.h
* QWebEngineView

and add the variable pointing to your location of the Qt install
```
export CUSTOM_WEB_ENGINE_DIR=Qt/5.13.2/Src/qtwebengine/src/webenginewidgets/api
```


### Command protocol
```bash
make proto
```

### Building browser and QPA
The makefile build both the browser and qpa (platform plugin). 

Build the source code in the root folder of the project

```
make install
```

### Building docker container

* Prerequisites: <br>
set QT login and token variables <br>

QT_CI_LOGIN=username <br>
QT_CI_PASSWORD=password <br>
PRIVATE_TOKEN_GITLAB=token
```
make docker
```





### Client
Go to the client folder and run the webserver

```
python3 -m http.server
```

### Start the browser

```
sh browser.sh
```



