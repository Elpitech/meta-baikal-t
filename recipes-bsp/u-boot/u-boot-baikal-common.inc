HOMEPAGE = "http://www.denx.de/wiki/U-Boot/WebHome"
SECTION = "bootloaders"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=c7383a594871c03da76b3707929d2919"

KBRANCH = "u-boot-${PV}-tp"
EXTERNALSRC = "${EXTERNAL_UBOOT_SRC}"
TPSDK_REPO ?= "gitlab.tpl"

SRC_URI = "git://${TPSDK_REPO}/core/u-boot.git;protocol=ssh;user=git;branch=${KBRANCH}"
SRCREV = "AUTOINC"

S = "${WORKDIR}/git"
B = "${WORKDIR}/build"