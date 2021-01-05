# notes for finding out why LocalStorage doesn't work


run docker
```bash
docker run -ti -v "/tmp/storage:/root/.local/share/browser/QtWebEngine/Default" -p 9001:9001 --name browser --rm  jimberbrowser bash
docker run -ti -v "/tmp/storage:/root/.local/share/browser/QtWebEngine/Default" -p 9002:9001 --name browser --rm  jimberbrowser bash
```