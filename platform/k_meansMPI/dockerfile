FROM ubuntu:14.04

RUN apt-get update && apt-get install -y libcr-dev mpich2 mpich2-doc

WORKDIR /var/

COPY . /var/

RUN ./run.sh
