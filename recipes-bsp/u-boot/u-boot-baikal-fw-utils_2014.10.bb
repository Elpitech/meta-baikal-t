SUMMARY = "U-Boot bootloader fw_printenv/setenv utilities"
DESCRIPTION = "U-boot firmware utilities specifically created to read/write environment image from Linux mtd-devices"
HOMEPAGE = "http://www.denx.de/wiki/U-Boot/WebHome"
SECTION = "bootloaders"
LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=c7383a594871c03da76b3707929d2919"
DEPENDS = "mtd-utils"

require u-boot-baikal.inc

INSANE_SKIP_${PN} = "already-stripped"
RPROVIDES_${PN} += "u-boot-fw-utils"
EXTRA_OEMAKE_class-target = 'CROSS_COMPILE=${TARGET_PREFIX} CC="${CC} ${CFLAGS} ${LDFLAGS}" HOSTCC="${BUILD_CC} ${BUILD_CFLAGS} ${BUILD_LDFLAGS}" V=1'
EXTRA_OEMAKE_class-cross = 'HOSTCC="${CC} ${CFLAGS} ${LDFLAGS}" V=1'

inherit uboot-config

do_compile () {
	oe_runmake -C ${S} O=${B} ${UBOOT_MACHINE}
	oe_runmake -C ${S} O=${B} env
}

do_install () {
	install -d ${D}${base_sbindir}
	install -d ${D}${sysconfdir}
	install -m 755 ${B}/tools/env/fw_printenv ${D}${base_sbindir}/fw_printenv
	ln -sv ${base_sbindir}/fw_printenv ${D}${base_sbindir}/fw_setenv
	install -m 0644 ${S}/tools/env/fw_env.config ${D}${sysconfdir}/fw_env.config
	if [ -n "${UBOOT_ENV_MTD_FILE}" ]; then
		echo -e "${UBOOT_ENV_MTD_FILE}\t0x00000000\t${UBOOT_ENV_SIZE}\t0x1000" > ${D}${sysconfdir}/fw_env.config
	fi
}

do_install_class-cross () {
	install -d ${D}${bindir_cross}
	install -m 755 ${B}/tools/env/fw_printenv ${D}${bindir_cross}/fw_printenv
	ln -sv ${bindir_cross}/fw_printenv ${D}${bindir_cross}/fw_setenv
}

SYSROOT_DIRS_append_class-cross = " ${bindir_cross}"

BBCLASSEXTEND = "cross"
