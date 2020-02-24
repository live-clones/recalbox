recalbox
========
![](https://s3-eu-west-1.amazonaws.com/forums.recalbox.com/8d81e556-aefb-4729-ad2f-9d7386eff4cd.png)

Welcome to the main recalbox repository.

This repository contains the source code that build the recalboxOS for different boards.

Recalbox is an opensource project. We hope that you will contribute and help us to improve this OS.
But if you are working on a fork, by respect for our work, we ask you not to integrate our work in progress located on branches other than master.
Thank you for waiting for a merge on master branch.

Please use **Issues** in corresponding projects to report a bug or request a feature.

## Recalbox Projects
- [recalbox/recalbox](https://gitlab.com/recalbox/recalbox): the repository contains the source code to build recalbox.
- [recalbox/recalbox-emulationstation](https://gitlab.com/recalbox/recalbox-emulationstation): the emulationstation frontend for recalbox.
- [recalbox/recalbox-configgen](https://gitlab.com/recalbox/recalbox-configgen): the tool for automatic joystick configuration.

## Useful links
- [www.recalbox.com](https://www.recalbox.com): the main recalbox website.
- [www.recalbox.com/blog/](https://www.recalbox.com/blog): the dev blog.
- [Recalbox Wiki](https://github.com/recalbox/recalbox-os/wiki): the wiki of recalbox.
- [forum.recalbox.com](https://forum.recalbox.com): recalbox forum. You will find support there.


# How to build ![Build status](https://gitlab.com/recalbox/recalbox/badges/master/pipeline.svg?style=flat-square)

### General steps

Install docker: [docs.docker.com/install/](https://docs.docker.com/install/)

Make sure your user belongs to the docker group -> `sudo usermod -a -G docker $USER` then logoff/login

Clone the repository in your home:

```bash
ARCH="rpi3" && git clone https://gitlab.com/recalbox/recalbox.git recalbox-${ARCH}
```

Run the script that compiles the project in a Docker container:

```bash
scripts/linux/recaldocker.sh
```

You can set the following environment variables to customise the build:
* `ARCH`: to force the target architecture (see available architectures in the [`configs` directory](configs), default is to infer it from the current directory name)
* `RECALBOX_VERSION`: to set a Recalbox build version (no impact on build, just the version shown)
* `PACKAGE`: if you want to build a single package

In the recalbox directory, you will find some directories created by the build:
* `host` folder that contains output compiled for your host
* `dl` folder that contains all packages download
* `output` folder that contains compiled files

### Custom command and menuconfig

Using the command line arguments, you can pass a custom command to run:
```bash
ARCH=rpi3 scripts/linux/recaldocker.sh make menuconfig
```

This way you can run menuconfig and configure the system. If you never built the system, use the following command to create the default configuration for your arch:
```bash
export ARCH=rpi3 && scripts/linux/recaldocker.sh make "recalbox-${ARCH}_defconfig" && make menuconfig
```

Your command will override the default build command from the docker image, so you may have to copy past some variable from it.

### Known errors
During the image built if you encounter errors like the following :

```text
Reading package lists...
W: Failed to fetch http://archive.ubuntu.com/ubuntu/dists/xenial/InRelease  Temporary failure resolving 'archive.ubuntu.com'
W: Failed to fetch http://archive.ubuntu.com/ubuntu/dists/xenial-updates/InRelease  Temporary failure resolving 'archive.ubuntu.com'
W: Failed to fetch http://archive.ubuntu.com/ubuntu/dists/xenial-backports/InRelease  Temporary failure resolving 'archive.ubuntu.com'
W: Failed to fetch http://security.ubuntu.com/ubuntu/dists/xenial-security/InRelease  Temporary failure resolving 'security.ubuntu.com'
W: Some index files failed to download. They have been ignored, or old ones used instead.
```
Docker cannot access to internet to make updates. More precisely, it is a DNS problem (see https://odino.org/cannot-connect-to-the-internet-from-your-docker-containers/). If your `/etc/resolv.conf` is empty (it happens if all is managed by `network-manager` for example), Docker does not know which DNS to use. You'll need to tell him through the `/etc/default/docker` file by adding this line :

```text
DOCKER_OPTS="--dns IP.OF.YOUR.DNS"
```
Restart Docker and retry. It should be ok.

```bash
sudo service docker restart
docker build -t "recalbox-dev" .
```
