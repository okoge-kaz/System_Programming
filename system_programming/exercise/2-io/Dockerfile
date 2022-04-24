#docker build -t sysprog ./
#docker run -it --rm -v `pwd`:/home/sysprog sysprog -c 'make test1'
FROM ubuntu
LABEL maintainer="hayashi@c.titech.ac.jp"

ARG TZ=UTC
ARG USER=sysprog
ARG GROUP=sysprog
ARG PASS=sysprog
ENV HOME=/home/${USER}

RUN apt-get update && apt-get clean \
  && DEBIAN_FRONTEND=noninteractive apt-get install -y tzdata \
  && ln -sf /usr/share/zoneinfo/${TZ} /etc/localtime \
  && dpkg-reconfigure --frontend noninteractive tzdata \
  && DEBIAN_FRONTEND=noninteractive apt-get install -y sudo build-essential clang binutils gdb strace bison flex ruby graphviz tmux telnet \
  && rm -rf /var/lib/apt/lists/* \
  && groupadd ${GROUP} \
  && useradd -g ${GROUP} -m ${USER} \
  && (echo "${USER}:${PASS}" | chpasswd) \
  && gpasswd -a ${USER} sudo

USER ${USER}
WORKDIR ${HOME}

ENTRYPOINT ["/bin/bash"]
