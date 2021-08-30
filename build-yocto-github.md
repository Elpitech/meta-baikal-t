Build guide
===========

## Prerequisites

The following packages are required on the build machine:

sudo apt install gawk wget git-core diffstat unzip texinfo gcc-multilib \
build-essential chrpath socat cpio python python3 python3-pip python3-pexpect \
xz-utils debianutils iputils-ping

The build procedure assumes password-less access to git, so add the key known to
your github account:

eval `ssh-agent -s`
ssh-add ~/.ssh/id_ed25519

## Checkout Yocto components

$ mkdir yocto
$ cd yocto

* **oe-core** - base Yocto layer, with functional classes of the build system
and reciped of the basic packages. Note that it is checked-out to the current
dir:
```
git clone git@github.com:Elpitech/oe-core.git .
```

* **bitbake** - set of python scripts for the build system. Check-out to
the directory named bitbake:
```
git clone git@github.com:Elpitech/bitbake.git bitbake
```

* **meta-baikal-t** - recipes for the cross toolchain, kernel and the U-Boot
  bootloader.
```
git clone git@github.com:Elpitech/meta-baikal-t.git meta-baikal-t
```

* **meta-recovery** - recipes to build the Yocto-based recovery ROM.
```
git clone git@github.com:Elpitech/meta-recovery.git meta-recovery
```

* **meta-openembedded** - recipes for verious useful utils that are not part
  of the base layer.
```
git clone git@github.com:Elpitech/meta-openembedded.git meta-openembedded
```

* **meta-micropython** - recipes for micropython build.
```
git clone git@github.com:Elpitech/meta-micropython.git meta-micropython
```

## Configure the build environment

Prepare the build environment script, for ex. work_yocto.env:
```
export TEMPLATECONF=meta-recovery/conf
# Supported targets: mitx, sfbt1, msbt2, mrbt1, mitx, rt1mb, azure, tc-sfbt1,
# tc-msbt2, cnc-sfbt1, cnc-msbt2, axt1-sfbt1, axt1-msbt2, bc3bt1-mrbt1, bfk,
# bfkx, bfk3
export MACHINE=mitx
export BB_NUMBER_THREADS=$(nproc)
export PARALLEL_MAKE="-j ${BB_NUMBER_THREADS}"
PS1=`echo $PS1 | sed -e 's/\['${MACHINE}'\] //g'`
PS1="[${MACHINE}] $PS1 "
# If your company uses proxy:
#export http_proxy='http://localhost:3128/'
#export https_proxy='https://localhost:3128/'
#export ftp_proxy='http://localhost:3128/'
#export ALL_PROXY='socks://localhost:3128/'
#export all_proxy='socks://localhost:3128/'
#export no_proxy='example.com'
```
Note, that you need to choose the appropriate build target in the script. In this case it is
'mitx'.

Source the build environment script
```
$ . ./work_yocto.env
```

Source the rest of the Yocto environment and build the Yocto ROM:
```
. ./oe-init-build-env
bitbake baikal-image-recovery
```

The build time vary. On a decent desktop it should not take more than 40
minutes to build the ROM. Upon completion the resulting ROM will be available in
./tmp/deploy/images/$MACHINE.

```
* baikal-image-recovery*.rootfs.cpio.gz - initramfs arhive;
* baikal-image-recovery*.rootfs.manifest - package list of initramfs;
* baikal-image-recovery*.rootfs.json - build time Yocto environment vars

* fitImage-*.bin - FIT-image binary that is to be verified and booted by u-boot. It includes
  the kernel, dtb, and the rootfs.
* fitImage-initramfs-*.bin - same as above but including initramfs;
* fitImage-*.its - dts-type script describing the FIT image;
* fitImage-initramfs-*.its - same but for initramfs;
* image-info-*.bin - binary blob with image info, used by recovery-image-info utility;
* modules-*.tgz - kernel modules, not always included in ROM;
* recovery-*.rom - the Yocto-based recovery ROM;
* sign-*.{crt, pub, key} - FIT-image sign keys. U-Boot will verify the
  signatures by default.
* tplatforms_*.dtb - DTB file used by the kernel;
* u-boot-*.bin - U-Boot binary;
* u-boot-mitx.dtb - DTB file used by U-Boot. For U-Boot 2014, only the rsa key
  is stored here;
* u-boot-env-*.txt - U-Boot env vars
* u-boot-env-*.bin - binary version of the above
* vmlinux-* - debug kernel image, non-bootable
* vmlinux-*.bin - kernel image without debug info, bootable
* vmlinuz-*.bin - compressed kernel image without debug info
```

## Optional settings of the build system

There are many parameters that can be used to further tune the building process.
Some of them are located in  build/conf/local.conf:

* MACHINE - target device name
* BB_NUMBER_THREADS - number of threads to use for parallel package building;
* PARALLEL_MAKE - parallel thread count used for the make's -j option;

## Building with external U-Boot and kernel sources

During the development it use more convinient to build Yocto ROM from some
working copy of U-Boot and kernel, not the release code commited to the repo.
If both U-Boot and kernel are external to the build system, set the
following variables:
```
export EXTERNAL_KERNEL_SRC=/<full>/<path>/<to>/<kernel>
export EXTERNAL_UBOOT_SRC=/<full>/<path>/<to>/<u-boot>
export BB_ENV_EXTRAWHITE="$BB_ENV_EXTRAWHITE EXTERNAL_KERNEL_SRC EXTERNAL_UBOOT_SRC"
```

By default the sources for the U-boot, kernel and our utilities are expected to
be at our github site. However, if you have a mirror of those git repos somewhere
else, then you can build from that mirror by configuring TPSDK_REPO variable:
```
export TPSDK_REPO="<repo-url-without-http-and-www>"
export BB_ENV_EXTRAWHITE="$BB_ENV_EXTRAWHITE TPSDK_REPO"
```

The mirror shall have the following repos: core/kernel, core/u-boot, core/cnc,
utils/flashrom, utils/recovery-image-info, utils/smt-89hpesX.

## How to rebuild if U-Boot and/or kernel sources have changed

```
The easy way:
bitbake -c cleanall baikal-image-recovery linux-yocto make-mod-scripts u-boot
rm -rf build/tmp/deploy/images/${MACHINE}
bitbake baikal-image-recovery

A more complicated way:
You can list tasks for each Yocto "package". Usually, it is 'fetch'
that you are looking for:
 bitbake -c listtasks u-boot
 bitbake -c listtasks u-boot-fw-utils
 bitbake -C fetch linux-yocto

So, to re-fetch, run:
 bitbake -C fetch u-boot-fw-utils
 bitbake -C fetch u-boot
 bitbake -C fetch linux-yocto
and then rebuild ROM:
 bitbake baikal-image-recovery
```

## How to build under docker

```
The Yocto Project 2.5  ("Sumo"), which is the base for our code, is too old to
be built on modern Linux (Debian 11). Specifically, GCC 10.x is too demanding to
the old Yocto code and fails to build the environment.  One option to run the
build in a docker containter. The docker image is prepared by us and is based on
Debian 10 which has an older GCC version. Download it and start the container:

$ docker run -it -d -v /home/ndz/t8/yocto:/mnt/srcroot --name bb xxor/buster-builder:1.0 /bin/bash
$ docker exec -it bb /bin/bash

Note that the above assumes the yocto home dir is mapped to /mnt/srcroot inside
the container.

Install prereqs:
# apt update
# apt install gawk wget git-core diffstat unzip texinfo gcc-multilib \
build-essential chrpath socat cpio python python3 python3-pip python3-pexpect \
xz-utils debianutils iputils-ping locales

# locale-gen en_US.UTF-8
# dpkg-reconfigure locales

Once inside the container, create user: 
# adduser ndz
# su - ndz

Create the ssh key and transfer the public part to your github profile.
$ ssh-keygen -o -a 100 -t ed25519 -f ~/.ssh/id_ed25519 -C "john@example.com"
(Note: Do not do this if the key is already present).

Add the ssh key that is known to github server:
$ eval `ssh-agent -s`
$ ssh-add ~/.ssh/id_ed25519

Then build yocto as usual:

$ cd /mnt/srcroot
$ . work_yocto.env
$ . ./oe-init-build-env
bitbake baikal-image-recovery
```
