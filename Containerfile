# Concoct - An imperative, dynamically-typed, interpreted, general-purpose programming language
# Copyright (c) 2020-2023 BlakeTheBlock and Lloyd Dilley
# http://concoct.ist/

FROM alpine:latest AS construct
RUN apk update && apk add clang cmake git make musl-dev && apk cache clean
RUN cd /root && git clone https://github.com/concoctist/concoct.git
RUN cd /root/concoct && mkdir bld && cd bld && cmake .. && make

FROM alpine:latest
RUN adduser -h /home/concoct -g "Concoct Account" -S concoct
USER concoct
WORKDIR /home/concoct
COPY --from=construct --chown=concoct /root/concoct/bld/bin/concoct .
CMD ["/home/concoct/concoct"]
