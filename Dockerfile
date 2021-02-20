from ubuntu:20.04

COPY test /tracer
COPY run_script.sh /run_script.sh

#ENTRYPOINT["/tracer"]