#!/bin/bash
ls -la
node index.js &
# nohup python3 app.py &>./python.log &
nginx -g "daemon off;"