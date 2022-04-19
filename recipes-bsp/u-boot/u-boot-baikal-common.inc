HOMEPAGE = "http://www.denx.de/wiki/U-Boot/WebHome"
DESCRIPTION = "U-Boot, a boot loader for Embedded boards based on PowerPC, \
ARM, MIPS and several other processors, which can be installed in a boot \
ROM and used to initialize and test the hardware or to download and run \
application code."
SECTION = "bootloaders"
DEPENDS += "flex-native bison-native"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=5a7450c57ffe5ae63fd732446b988025"
PE = "1"

SRCREV = "AUTOINC"
EXTERNALSRC = "${EXTERNAL_UBOOT_SRC}"

TPSDK_REPO ?= "gitlab.elpitech.ru"
KBRANCH = "v${PV}elp"
SRC_URI = "git://${TPSDK_REPO}/baikal-t/u-boot.git;protocol=ssh;user=git;branch=${KBRANCH}"

S = "${WORKDIR}/git"
B = "${WORKDIR}/build"
do_configure[cleandirs] = "${B}"
