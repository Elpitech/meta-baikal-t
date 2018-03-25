DEFAULTTUNE ?= "mips32r5"

require conf/machine/include/tune-mips32.inc

# Add the CCFLAGS to have software properly optimized for MIPS32r5 arch
TUNEVALID[mips32r5] = "Enable mips32r5 specific processor optimizations"
TUNECONFLICTS[mips32r5] = "n64 n32"
TUNE_CCARGS .= "${@bb.utils.contains('TUNE_FEATURES', 'mips32r5', ' -march=mips32r5', '', d)}"

# Base Tunes (Hard Float)
AVAILTUNES += "mips32r5 mips32r5el"

TUNE_FEATURES_tune-mips32r5 = "${TUNE_FEATURES_tune-mips} mips32r5"
MIPSPKGSFX_VARIANT_tune-mips32r5 = "mips32r5"
PACKAGE_EXTRA_ARCHS_tune-mips32r5 = "mips mips32 mips32r5"

TUNE_FEATURES_tune-mips32r5el = "${TUNE_FEATURES_tune-mipsel} mips32r5"
MIPSPKGSFX_VARIANT_tune-mips32r5el = "mips32r5el"
PACKAGE_EXTRA_ARCHS_tune-mips32r5el = "mipsel mips32el mips32r5el"

# Soft Float
AVAILTUNES += "mips32r5-nf mips32r5el-nf"

TUNE_FEATURES_tune-mips32r5-nf = "${TUNE_FEATURES_tune-mips-nf} mips32r5"
MIPSPKGSFX_VARIANT_tune-mips32r5-nf = "mips32r5"
PACKAGE_EXTRA_ARCHS_tune-mips32r5-nf = "mips-nf mips32-nf mips32r5-nf"

TUNE_FEATURES_tune-mips32r5el-nf = "${TUNE_FEATURES_tune-mipsel-nf} mips32r5"
MIPSPKGSFX_VARIANT_tune-mips32r5el-nf = "mips32r5el"
PACKAGE_EXTRA_ARCHS_tune-mips32r5el-nf = "mipsel-nf mips32el-nf mips32r5el-nf"

# Set GCC default build settings
MIPS32R5_OECONF    += "--with-abi=32 --with-arch=mips32r5 --with-float=hard"
MIPS32R5_NF_OECONF += "--with-abi=32 --with-arch=mips32r5 --with-float=soft"
EXTRA_OECONF_append_pn-gcc  = " ${@'${MIPS32R5_OECONF}' if '${DEFAULTTUNE}' in ['mips32r5', 'mips32r5el'] else ''}"
EXTRA_OECONF_append_pn-gcc += " ${@'${MIPS32R5_NF_OECONF}' if '${DEFAULTTUNE}' in ['mips32r5-nf', 'mips32r5el-nf'] else ''}"
EXTRA_OECONF_append_pn-gcc-cross-initial-${TARGET_ARCH}  = " ${@'${MIPS32R5_OECONF}' if '${DEFAULTTUNE}' in ['mips32r5', 'mips32r5el'] else ''}"
EXTRA_OECONF_append_pn-gcc-cross-initial-${TARGET_ARCH} += " ${@'${MIPS32R5_NF_OECONF}' if '${DEFAULTTUNE}' in ['mips32r5-nf', 'mips32r5el-nf'] else ''}"
EXTRA_OECONF_append_pn-gcc-cross-${TARGET_ARCH}  = " ${@'${MIPS32R5_OECONF}' if '${DEFAULTTUNE}' in ['mips32r5', 'mips32r5el'] else ''}"
EXTRA_OECONF_append_pn-gcc-cross-${TARGET_ARCH} += " ${@'${MIPS32R5_NF_OECONF}' if '${DEFAULTTUNE}' in ['mips32r5-nf', 'mips32r5el-nf'] else ''}"
EXTRA_OECONF_append_pn-gcc-cross-canadian-${TARGET_ARCH}  = " ${@'${MIPS32R5_OECONF}' if '${DEFAULTTUNE}' in ['mips32r5', 'mips32r5el'] else ''}"
EXTRA_OECONF_append_pn-gcc-cross-canadian-${TARGET_ARCH} += " ${@'${MIPS32R5_NF_OECONF}' if '${DEFAULTTUNE}' in ['mips32r5-nf', 'mips32r5el-nf'] else ''}"
