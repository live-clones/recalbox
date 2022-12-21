FROM ubuntu:22.04
LABEL maintainer="digitalLumberjack <digitallumberjack@recalbox.com>"

ENV TERM xterm
ENV ARCH ''
ENV RECALBOX_VERSION 'development'
ENV RECALBOX_CCACHE_ENABLED ''
ENV PACKAGE ''

# Install dependencies
# needed ? xterm
RUN apt-get update -y && \
apt-get install -y tzdata apt-utils && \
ln -fs /usr/share/zoneinfo/Europe/Paris /etc/localtime && \
dpkg-reconfigure --frontend noninteractive tzdata && \
apt-get -y install build-essential git libncurses5-dev qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools \
mercurial libdbus-glib-1-dev texinfo zip openssh-client libxml2-utils libpng-dev \
software-properties-common wget cpio bc locales rsync imagemagick bison flex bsdmainutils \
nano vim automake autopoint mtools dosfstools subversion openjdk-8-jdk libssl-dev libelf-dev \
graphviz python3 python3-matplotlib python3-numpy python3-six re2c libc6-dev-i386 libtool cabextract
RUN curl -sL https://deb.nodesource.com/setup_12.x | bash - && apt-get -y install nodejs yarn
RUN curl -sS https://dl.yarnpkg.com/debian/pubkey.gpg | apt-key add - && echo "deb https://dl.yarnpkg.com/debian/ stable main" | tee /etc/apt/sources.list.d/yarn.list && apt update && apt install -y yarn
RUN yarn global add @quasar/cli
RUN rm -rf /var/lib/apt/lists/*

# Set the locale needed by toolchain
RUN echo "en_US.UTF-8 UTF-8" > /etc/locale.gen
RUN locale-gen

RUN mkdir -p /work
WORKDIR /work

CMD echo ">>> Setting recalbox version to ${RECALBOX_VERSION}" && echo "${RECALBOX_VERSION}" > board/recalbox/fsoverlay/recalbox/recalbox.version && \
    echo ">>> Reseting buildroot submodule" && ( cd buildroot && git reset HEAD --hard && git clean -dfx ) && \
    echo ">>> Fetching buildroot submodule" && ( git submodule update --init ) && \
    echo ">>> Making recalbox-${ARCH}_defconfig" && make recalbox-${ARCH}_defconfig && \
    export RECALBOX_CCACHE=${RECALBOX_CCACHE_ENABLED:+"BR2_CCACHE=y BR2_CCACHE_DIR=/share/ccache BR2_CCACHE_INITIAL_SETUP=--max-size=500G BR2_CCACHE_USE_BASEDIR=y"} && \
    echo ">>> Make with command : BR2_DL_DIR="/share/dl" $RECALBOX_CCACHE $PACKAGE" && \
    make BR2_DL_DIR="/share/dl" $RECALBOX_CCACHE $PACKAGE
