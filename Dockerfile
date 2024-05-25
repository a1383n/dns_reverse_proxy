FROM ubuntu:latest

RUN apt-get update && apt-get install -y build-essential cmake pkg-config libcurl4-openssl-dev libldns-dev libjsoncpp-dev libgflags-dev

RUN mkdir /opt/source && mkdir /opt/source/build

WORKDIR /opt/source

COPY . .

RUN cmake -S . -B ./build && cmake --build ./build --target dns_reverse_proxy -j $(nproc) && chmod +x ./build/dns_reverse_proxy