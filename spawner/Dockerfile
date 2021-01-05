FROM ubuntu:20.04 as builder

RUN apt-get update && apt-get install -y iputils-ping curl gnupg2

RUN curl -sL https://deb.nodesource.com/setup_12.x | bash -
RUN apt-get install nodejs -y
RUN npm i -g yarn
COPY src build/.
COPY tsconfig.json build/
COPY package.json build/
WORKDIR build
RUN yarn install
RUN yarn build

FROM nginx:latest

RUN mkdir -p /opt/jimber/browser
WORKDIR /opt/jimber/browser

RUN apt-get update && apt-get install -y --no-install-recommends iputils-ping \
    curl gnupg2 \
    && rm -rf /var/lib/apt/lists/*

RUN curl -sL https://deb.nodesource.com/setup_12.x | bash -
RUN apt update && apt install nodejs -y

ENV DOCKERVERSION=19.03.5
RUN curl -fsSLO https://download.docker.com/linux/static/stable/x86_64/docker-${DOCKERVERSION}.tgz \
    && tar xzvf docker-${DOCKERVERSION}.tgz --strip 1 \
    -C /usr/local/bin docker/docker \
    && rm docker-${DOCKERVERSION}.tgz

COPY dist .
COPY --from=builder build/dist/* ./
COPY --from=builder build/node_modules ./node_modules
RUN mkdir modules
COPY nginx.conf /etc/nginx/nginx.conf
COPY Routes /etc/nginx/Routes/


COPY start.sh start.sh
RUN chmod +x start.sh
CMD ./start.sh