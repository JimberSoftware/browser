Do you see something in the logs and can't find out where it's being logged? use:
```bash
export QT_MESSAGE_PATTERN="(%{file}:%{line}) - %{message}"
```



To compile we need some extra headers from customwebengine
```bash
export CUSTOM_WEB_ENGINE_DIR=/home/amol/Qt/video/src/webenginewidgets/api/
```