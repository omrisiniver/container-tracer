FROM ubuntu:20.04 as builder
ENV DEBIAN_FRONTEND noninteractive

WORKDIR /app
RUN apt-get update && apt-get -y install gcc g++ cmake make
COPY . /app
RUN cmake .
RUN make tracer

FROM scratch
COPY --from=builder /app/tracer tracer
COPY docker/run_script.sh /app/run_script.sh

# CMD["/run_script.sh"]