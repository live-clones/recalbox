FROM ubuntu:18.04
LABEL maintainer="digitalLumberjack <digitallumberjack@recalbox.com>"

ENV TERM xterm
ENV DEBIAN_FRONTEND=noninteractive
ENV ARCH ''
ENV RECALBOX_VERSION 'development'
ENV RECALBOX_CCACHE_ENABLED ''
ENV PACKAGE ''

# Install dependencies
RUN apt-get update -y && \
# Configure time zone as UTC
apt-get install -y tzdata && \
ln -fs /usr/share/zoneinfo/Etc/UTC /etc/localtime && \
dpkg-reconfigure --frontend noninteractive tzdata && \
# Buildroot mandatory dependencies
apt-get -y install sed make binutils build-essential gcc g++ bash patch gzip bzip2 perl tar cpio python unzip rsync file bc wget \
# Buildroot optional dependencies: source fetching tools
git mercurial subversion openjdk-8-jdk \
# Buildroot optional dependencies: java (kodi selects BR2_NEEDS_JAVA_HOST)
openjdk-8-jdk \
# Buildroot optional dependencies: graph generation tools
graphviz python-matplotlib \
# Imagemagik is used in the post-build script
imagemagick \
# To set the locale needed by the toochain
locales && \
# Clean-up apt cache
apt-get clean && \
rm -rf /var/lib/apt/lists/*

# Default time zone is UTC: nothing to do

# Set the locale needed by toolchain
RUN echo "en_US.UTF-8 UTF-8" > /etc/locale.gen
RUN locale-gen

WORKDIR /work

CMD echo ">>> Setting recalbox version to ${RECALBOX_VERSION}" && echo "${RECALBOX_VERSION}" > board/recalbox/fsoverlay/recalbox/recalbox.version && \
    echo ">>> Fetching and reseting buildroot submodule" && ( git submodule update --init ; cd buildroot && git reset HEAD --hard && git clean -dfx ) && \
    echo ">>> Making recalbox-${ARCH}_defconfig" && make recalbox-${ARCH}_defconfig && \
    export RECALBOX_CCACHE=${RECALBOX_CCACHE_ENABLED:+"BR2_CCACHE=y BR2_CCACHE_DIR=/share/ccache BR2_CCACHE_INITIAL_SETUP=--max-size=500G BR2_CCACHE_USE_BASEDIR=y"} && \
    echo ">>> Make with command : BR2_DL_DIR="/share/dl" $RECALBOX_CCACHE $PACKAGE" && \
    make BR2_DL_DIR="/share/dl" $RECALBOX_CCACHE $PACKAGE
