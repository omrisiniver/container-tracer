FROM tracer as tracer

FROM ubuntu:20.04

WORKDIR /app
COPY --from=tracer /tracer /app/tracer
COPY --from=tracer /app/run_script.sh /app/run_script.sh
RUN chmod +x /app/run_script.sh

COPY . /app
ENV PROC test

CMD ["/app/run_script.sh"]