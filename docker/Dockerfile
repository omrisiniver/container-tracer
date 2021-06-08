FROM ubuntu:20.04
ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && apt-get -y install gcc g++   


COPY tracer /tracer
COPY run_script.sh /run_script.sh

# CMD["/run_script.sh"]