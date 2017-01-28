FROM ubuntu:16.04

RUN apt-get update && apt-get install -y libsdl2-dev libsdl2-mixer-dev libboost-all-dev libfreeimage-dev libfreetype6-dev libeigen3-dev libcurl4-openssl-dev libasound2-dev libgl1-mesa-dev build-essential cmake

WORKDIR /src

ADD . /src

RUN cmake .

RUN make -j8
