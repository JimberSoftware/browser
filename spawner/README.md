# Spawner of jimberbrowser

In this module resides a simple api which spawns a jimberbrowser and returns a container id upon request.

## Running

[venv is advised](https://docs.python.org/3/library/venv.html#creating-virtual-environments)

```bash
pip install -r requirements.txt

python app.py
```

## Building (docker)

```bash
make spawner
```

## Running (docker)

```bash
docker run -ti -v /var/run/docker.sock:/var/run/docker.sock -p 8000:80 --network jimberbrowser --rm --name spawner jimberbrowser_spawner
```