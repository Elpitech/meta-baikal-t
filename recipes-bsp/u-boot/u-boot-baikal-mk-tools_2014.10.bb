SUMMARY = "U-Boot bootloader image creation tools"
DESCRIPTION = "U-boot mk(env)image tools used to generate bootloader-specific binaries"
HOMEPAGE = "http://www.denx.de/wiki/U-Boot/WebHome"
SECTION = "bootloaders"
LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=c7383a594871c03da76b3707929d2919"
DEPENDS = "openssl"

require u-boot-baikal.inc

PROVIDES_append_class-target = " u-boot-mkimage"
EXTRA_OEMAKE_class-target = 'CROSS_COMPILE="${TARGET_PREFIX}" CC="${CC} ${CFLAGS} ${LDFLAGS}" HOSTCC="${BUILD_CC} ${BUILD_CFLAGS} ${BUILD_LDFLAGS}" STRIP=true V=1'

PROVIDES_append_class-native = " u-boot-mkimage-native"
EXTRA_OEMAKE_class-native = 'CC="${BUILD_CC} ${BUILD_CFLAGS} ${BUILD_LDFLAGS}" HOSTCC="${BUILD_CC} ${BUILD_CFLAGS} ${BUILD_LDFLAGS}" STRIP=true V=1'

PROVIDES_append_class-nativesdk = " u-boot-mkimage-nativesdk"
EXTRA_OEMAKE_class-nativesdk = 'CROSS_COMPILE="${HOST_PREFIX}" CC="${CC} ${CFLAGS} ${LDFLAGS}" HOSTCC="${BUILD_CC} ${BUILD_CFLAGS} ${BUILD_LDFLAGS}" STRIP=true V=1'

do_compile () {
	oe_runmake -C ${S} O=${B} sandbox_defconfig

	# Disable CONFIG_CMD_LICENSE, license.h is not used by tools and
	# generating it requires bin2header tool, which for target build
	# is built with target tools and thus cannot be executed on host.
	sed -i "s/CONFIG_CMD_LICENSE=.*/# CONFIG_CMD_LICENSE is not set/" .config

	oe_runmake -C ${S} O=${B} cross_tools NO_SDL=1
}

do_install () {
	install -d ${D}${bindir}
	install -m 0755 tools/mkimage ${D}${bindir}/uboot-mkimage
	install -m 0755 tools/mkenvimage ${D}${bindir}/uboot-mkenvimage
	ln -sf uboot-mkimage ${D}${bindir}/mkimage
	ln -sf uboot-mkenvimage ${D}${bindir}/mkenvimage
}

BBCLASSEXTEND = "native nativesdk"
