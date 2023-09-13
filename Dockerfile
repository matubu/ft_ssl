FROM ubuntu:latest

RUN apt-get update && apt-get install -y afl++

# create a new user
RUN useradd -ms /bin/bash fuzz

# switch to the new user
USER fuzz

COPY . /home/fuzz
WORKDIR /home/fuzz/