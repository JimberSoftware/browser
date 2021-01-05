.PHONY: docker spawner jimberbrowser proto #so make doesn't say "'*' is up to date."

build:
	cd browser && qmake && make
	cd qpa && qmake && make

install: build
	cd qpa && make install

jimberbrowser: browserdist
	cd docker && docker build --build-arg QT_CI_LOGIN=${QT_CI_LOGIN} --build-arg QT_CI_PASSWORD=${QT_CI_PASSWORD} --build-arg PRIVATE_TOKEN_GITLAB=${PRIVATE_TOKEN_GITLAB} -t jimberbrowser .

browserdist: build
	mkdir ./docker/bins -p
	mkdir ./docker/libs -p
	mkdir ./docker/customqtwebengine -p
	cp ./browser/build/browser ./docker/bins/browser
	cp ./qpa/build/libJimberQPA.so ./docker/libs/libJimberQPA.so

docker: jimberbrowser

client:
	echo "some vue stuff"

spawner:
	rm -rf spawner/dist || true
	mkdir spawner/dist || true
	cp -r client spawner/dist/www
	cp spawner/dist/www/config_local.js spawner/dist/www/config.js
	cd spawner && docker build . -t jimberbrowser_spawner

spawnerold:
	rm -rf spawnerold/dist || true
	mkdir spawnerold/dist || true
	cp -r client spawnerold/dist/www
	cp spawnerold/dist/www/config_local.js spawnerold/dist/www/config.js
	cd spawnerold && docker build . -t jimberbrowser_spawner

dev: #this starts a 4 panel tmux, a panel for each component
	gnome-terminal --window -e 'tmux new-session \; split-window -d \; split-window -h \; select-pane -t 2 \; split-window -h \; select-pane -t 0 \; send-keys "cd browser" ENTER \; select-pane -t 1 \; send-keys "cd qpa" ENTER \; select-pane -t 2 \; send-keys "cd client && http-server" ENTER'

proto:
	cd proto && ./generate.py

start-nginx:
	nginx -g "daemon off;" -c `pwd`/nginx.dev.conf

all: proto install spawner docker