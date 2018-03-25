SUMMARY = "Universal Boot Loader for embedded devices"
DESCRIPTION = "Custom U-boot source code specifically patched for Baikal-T based devices"
HOMEPAGE = "http://www.denx.de/wiki/U-Boot/WebHome"
SECTION = "bootloaders"
LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=c7383a594871c03da76b3707929d2919"

require recipes-bsp/u-boot/u-boot.inc
require u-boot-baikal.inc

inherit uboot-env

DEPENDS += "bc-native dtc-native"

UBOOT_BASE_NAME ??= "${PKGE}-${PKGV}-${PKGR}-${MACHINE}-${DATETIME}"
UBOOT_SYMLINK_NAME ??= "${MACHINE}"
UBOOT_BASE_NAME[vardepsexclude] += "DATETIME"

# Override do_deploy completely. We don't need so many symbolic links as well as
# the UBOOT_ENV functionality (we got uboot-env for it).
do_deploy () {
    if [ -n "${UBOOT_CONFIG}" ]
    then
        for config in ${UBOOT_MACHINE}; do
            i=$(expr $i + 1);
            for type in ${UBOOT_CONFIG}; do
                j=$(expr $j + 1);
                if [ $j -eq $i ]
                then
                    install -d ${DEPLOYDIR}
                    install -m 644 ${B}/${config}/u-boot-${type}.${UBOOT_SUFFIX} ${DEPLOYDIR}/u-boot-${type}-${UBOOT_BASE_NAME}.${UBOOT_SUFFIX}
                    ln -sf u-boot-${type}-${UBOOT_BASE_NAME}.${UBOOT_SUFFIX} ${DEPLOYDIR}/u-boot-${type}-${UBOOT_SYMLINK_NAME}.${UBOOT_SUFFIX}
                fi
            done
            unset  j
        done
        unset  i
    else
        install -d ${DEPLOYDIR}
        install -m 644 ${B}/${UBOOT_BINARY} ${DEPLOYDIR}/u-boot-${UBOOT_BASE_NAME}.${UBOOT_SUFFIX}
        ln -sf u-boot-${UBOOT_BASE_NAME}.${UBOOT_SUFFIX} ${DEPLOYDIR}/u-boot-${UBOOT_SYMLINK_NAME}.${UBOOT_SUFFIX}
    fi

    if [ -n "${UBOOT_ELF}" ]
    then
        if [ -n "${UBOOT_CONFIG}" ]
        then
            for config in ${UBOOT_MACHINE}; do
                i=$(expr $i + 1);
                for type in ${UBOOT_CONFIG}; do
                    j=$(expr $j + 1);
                    if [ $j -eq $i ]
                    then
                        install -m 644 ${B}/${config}/${UBOOT_ELF} ${DEPLOYDIR}/u-boot-${type}-${UBOOT_BASE_NAME}.${UBOOT_ELF_SUFFIX}
                        ln -sf u-boot-${type}-${UBOOT_BASE_NAME}.${UBOOT_ELF_SUFFIX} \
                               ${DEPLOYDIR}/u-boot-${type}-${UBOOT_SYMLINK_NAME}.${UBOOT_ELF_SUFFIX}
                    fi
                done
                unset j
            done
            unset i
        else
            install -m 644 ${B}/${UBOOT_ELF} ${DEPLOYDIR}/u-boot-${UBOOT_BASE_NAME}.${UBOOT_ELF_SUFFIX}
            ln -sf u-boot-${UBOOT_BASE_NAME}.${UBOOT_ELF_SUFFIX} ${DEPLOYDIR}/u-boot-${UBOOT_SYMLINK_NAME}.${UBOOT_ELF_SUFFIX}
        fi
    fi

    if [ -n "${SPL_BINARY}" ]
    then
        if [ -n "${UBOOT_CONFIG}" ]
        then
            for config in ${UBOOT_MACHINE}; do
                i=$(expr $i + 1);
                for type in ${UBOOT_CONFIG}; do
                    j=$(expr $j + 1);
                    if [ $j -eq $i ]
                    then
                        install -m 644 ${B}/${config}/${SPL_BINARY} ${DEPLOYDIR}/${SPL_BINARYNAME}-${type}-${UBOOT_BASE_NAME}.spl
                        ln -sf ${SPL_BINARYNAME}-${type}-${UBOOT_BASE_NAME}.spl \
                               ${DEPLOYDIR}/${SPL_BINARYNAME}-${type}-${UBOOT_SYMLINK_NAME}.spl
                    fi
                done
                unset  j
            done
            unset  i
        else
            install -m 644 ${B}/${SPL_BINARY} ${DEPLOYDIR}/${SPL_BINARYNAME}-${UBOOT_BASE_NAME}.spl
            ln -sf ${SPL_BINARYNAME}-${UBOOT_BASE_NAME}.spl ${DEPLOYDIR}/${SPL_BINARYNAME}-${UBOOT_SYMLINK_NAME}.spl
        fi
    fi
}

# Don't really need so many dtb links. Lets override the do_deploy_dtb method
# creating just the necessary ones.
do_deploy_dtb () {
        mkdir -p ${DEPLOYDIR}

        if [ -f ${B}/u-boot.dtb ]; then
                install ${B}/u-boot.dtb ${DEPLOYDIR}/u-boot-${UBOOT_BASE_NAME}.dtb
                ln -sf u-boot-${UBOOT_BASE_NAME}.dtb ${DEPLOYDIR}/u-boot-${UBOOT_SYMLINK_NAME}.dtb
        fi
        if [ -f ${B}/${UBOOT_NODTB_BINARY} ]; then
                install ${B}/${UBOOT_NODTB_BINARY} ${DEPLOYDIR}/u-boot-nodtb-${UBOOT_BASE_NAME}.${UBOOT_SUFFIX}
                ln -sf u-boot-nodtb-${UBOOT_BASE_NAME}.${UBOOT_SUFFIX} ${DEPLOYDIR}/u-boot-nodtb-${UBOOT_SYMLINK_NAME}.${UBOOT_SUFFIX}
        fi
}

do_concat_dtb () {
        # Concatenate U-Boot w/o DTB & DTB with public key
        # (cf. kernel-fitimage.bbclass for more details)
        if [ "x${UBOOT_SIGN_ENABLE}" = "x1" ]; then
                if [ "x${UBOOT_SUFFIX}" = "ximg" -o "x${UBOOT_SUFFIX}" = "xrom" ] && \
                   [ -e "${DEPLOYDIR}/u-boot-${UBOOT_BASE_NAME}.dtb" ]; then
                        cd ${B}
                        oe_runmake EXT_DTB=${DEPLOYDIR}/u-boot-${UBOOT_BASE_NAME}.dtb
                        install ${B}/${UBOOT_BINARY} ${DEPLOYDIR}/u-boot-${UBOOT_BASE_NAME}.${UBOOT_SUFFIX}
                        install ${B}/${UBOOT_BINARY} ${DEPLOY_DIR_IMAGE}/u-boot-${UBOOT_BASE_NAME}.${UBOOT_SUFFIX}
                elif [ -e "${DEPLOYDIR}/u-boot-nodtb-${UBOOT_BASE_NAME}.${UBOOT_SUFFIX}" ]; then
                        bbnote "DTB with public key is already concatenated with U-boot"
                else
                        bbnote "DTB must be embedded to U-boot binary (CONFIG_OF_EMBED), otherwise verified boot won't work."
                fi
        fi
}
