DEFAULTTUNE ?= "mips32r5"

require conf/machine/include/mips/tune-mips32r2.inc

# Add the CCFLAGS to have software properly optimized for MIPS32r5 arch
TUNEVALID[mips32r5] = "Enable mips32r5 specific processor optimizations"
TUNECONFLICTS[mips32r5] = "n64 n32"
TUNE_CCARGS .= "${@bb.utils.contains('TUNE_FEATURES', 'mips32r5', ' -march=mips32r5', '', d)}"

# Base Tunes (Hard Float)
AVAILTUNES += "mips32r5 mips32r5el"

TUNE_FEATURES:tune-mips32r5 = "${TUNE_FEATURES:tune-mips} mips32r5"
MIPSPKGSFX_VARIANT:tune-mips32r5 = "mips32r5"
PACKAGE_EXTRA_ARCHS:tune-mips32r5 = "mips mips32 mips32r2 mips32r5"

TUNE_FEATURES:tune-mips32r5el = "${TUNE_FEATURES:tune-mipsel} mips32r5"
MIPSPKGSFX_VARIANT:tune-mips32r5el = "mips32r5el"
PACKAGE_EXTRA_ARCHS:tune-mips32r5el = "mipsel mips32el mips32r2el mips32r5el"

# Soft Float
AVAILTUNES += "mips32r5-nf mips32r5el-nf"

TUNE_FEATURES:tune-mips32r5-nf = "${TUNE_FEATURES:tune-mips-nf} mips32r5"
MIPSPKGSFX_VARIANT:tune-mips32r5-nf = "mips32r5"
PACKAGE_EXTRA_ARCHS:tune-mips32r5-nf = "mips-nf mips32-nf mips32r2-nf mips32r5-nf"

TUNE_FEATURES:tune-mips32r5el-nf = "${TUNE_FEATURES:tune-mipsel-nf} mips32r5"
MIPSPKGSFX_VARIANT:tune-mips32r5el-nf = "mips32r5el"
PACKAGE_EXTRA_ARCHS:tune-mips32r5el-nf = "mipsel-nf mips32el-nf mips32r2el-nf mips32r5el-nf"

# Set GCC default build settings
MIPS32R5_OECONF  = "${@'--with-abi=32 --with-arch=mips32r5' if '${DEFAULTTUNE}' in ['mips32r5', 'mips32r5el', 'mips32r5-nf', 'mips32r5el-nf'] else ''}"
MIPS32R5_OECONF += "${@'--with-float=hard --with-fp-32=xx' if '${DEFAULTTUNE}' in ['mips32r5', 'mips32r5el'] else ''}"
MIPS32R5_OECONF += "${@'--with-float=soft' if '${DEFAULTTUNE}' in ['mips32r5-nf', 'mips32r5el-nf'] else ''}"
MIPS32R5_OECONF += "--enable-target-optspace --enable-plugin --with-mips-plt"
EXTRA_OECONF:append:pn-gcc = " ${MIPS32R5_OECONF}"
EXTRA_OECONF:append:pn-gcc-cross-initial-${TARGET_ARCH} = " ${MIPS32R5_OECONF}"
EXTRA_OECONF:append:pn-gcc-cross-${TARGET_ARCH} = " ${MIPS32R5_OECONF}"
EXTRA_OECONF:append:pn-gcc-cross-canadian-${TARGET_ARCH} = " ${MIPS32R5_OECONF}"
