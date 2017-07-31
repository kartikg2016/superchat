FROM ubuntu:latest

RUN apt-get update && apt-get install -y zlib1g-dev build-essential wget libboost-all-dev libncurses-dev

RUN mkdir /opt/superchat

COPY . /opt/superchat/

WORKDIR /opt/superchat/

ENV OSPL_HOME=HDE/x86_64.linux
ENV PATH=$OSPL_HOME/bin:$PATH
ENV LD_LIBRARY_PATH=$OSPL_HOME/lib${LD_LIBRARY_PATH:+:}$LD_LIBRARY_PATH
ENV CPATH=$OSPL_HOME/include:$OSPL_HOME/include/sys:$CPATH
ENV OSPL_URI=file://$OSPL_HOME/etc/config/ospl.xml
ENV OSPL_TMPL_PATH=$OSPL_HOME/etc/idlpp

RUN make

EXPOSE 7400-7411

ENTRYPOINT [ "./bin/superchat" ]

