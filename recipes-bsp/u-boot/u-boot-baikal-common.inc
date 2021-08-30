HOMEPAGE = "http://www.denx.de/wiki/U-Boot/WebHome"
SECTION = "bootloaders"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=${@'c7383a594871c03da76b3707929d2919' \
	if '${PV}' == '2014.10' else '30503fd321432fc713238f582193b78e'}"

KBRANCH = "u-boot-${PV}-tp"
EXTERNALSRC = "${EXTERNAL_UBOOT_SRC}"
TPSDK_REPO ?= "github.com"

SRC_URI = "git://${TPSDK_REPO}/Elpitech/baikal-t-u-boot.git;protocol=ssh;user=git;branch=${KBRANCH}"
SRCREV = "AUTOINC"

S = "${WORKDIR}/git"
B = "${WORKDIR}/build"
