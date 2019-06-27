DEFAULTTUNE ?= "p5600"

require conf/machine/include/tune-mips32r5.inc

# MIPS Warrior P5600 tuning feature
TUNEVALID[p5600] = "MIPS Warrior P-class P5600 Multiprocessor core"
TUNECONFLICTS[p5600] = "n64 n32"
TUNE_CCARGS .= "${@bb.utils.contains('TUNE_FEATURES', 'p5600', ' -march=mips32r5 -mtune=p5600', '', d)}"

# Base Tunes (Hard Float)
AVAILTUNES += "p5600 p5600el"

TUNE_FEATURES_tune-p5600 = "${TUNE_FEATURES_tune-mips} p5600"
MIPSPKGSFX_VARIANT_tune-p5600 = "p5600"
PACKAGE_EXTRA_ARCHS_tune-p5600 = "mips mips32 mips32r2 mips32r5 p5600"

TUNE_FEATURES_tune-p5600el = "${TUNE_FEATURES_tune-mipsel} p5600"
MIPSPKGSFX_VARIANT_tune-p5600el = "p5600el"
PACKAGE_EXTRA_ARCHS_tune-p5600el = "mipsel mips32el mips32r2el mips32r5el p5600el"

# Soft Float
AVAILTUNES += "p5600-nf p5600el-nf"

TUNE_FEATURES_tune-p5600-nf = "${TUNE_FEATURES_tune-mips-nf} p5600"
MIPSPKGSFX_VARIANT_tune-p5600-nf = "p5600"
PACKAGE_EXTRA_ARCHS_tune-p5600-nf = "mips-nf mips32-nf mips32r2-nf mips32r5-nf p5600-nf"

TUNE_FEATURES_tune-p5600el-nf = "${TUNE_FEATURES_tune-mipsel-nf} p5600"
MIPSPKGSFX_VARIANT_tune-p5600el-nf = "p5600el"
PACKAGE_EXTRA_ARCHS_tune-p5600el-nf = "mipsel-nf mips32el-nf mips32r2el-nf mips32r5el-nf p5600el-nf"

# MIPS 64-bits floating point registers feature
TUNEVALID[fp64] = "Use MIPS 64-bits FP registers"
TUNE_CCARGS .= "${@bb.utils.contains('TUNE_FEATURES', 'fp64', ' -mfp64', '', d)}"

# MIPS eXtended Physical Address (XPA) instructions
TUNEVALID[xpa] = "Use MIPS XPA instructions"
TUNE_CCARGS .= "${@bb.utils.contains('TUNE_FEATURES', 'xpa', ' -mxpa', '', d)}"

# MIPS Enhanced Virtual Addressing instructions
TUNEVALID[eva] = "Use MIPS EVA instructions"
TUNE_CCARGS .= "${@bb.utils.contains('TUNE_FEATURES', 'eva', ' -meva', '', d)}"

# MIPS Virtualization (VZ) instructions
TUNEVALID[virt] = "Use MIPS VZ instructions"
TUNE_CCARGS .= "${@bb.utils.contains('TUNE_FEATURES', 'virt', ' -mvirt', '', d)}"

# MIPS SIMD registers and instructions
TUNEVALID[msa] = "Use MIPS SIMD instructions"
TUNE_P5600_MSA = ""
TUNE_CCARGS .= "${@bb.utils.contains('TUNE_FEATURES', 'msa', ' ${TUNE_P5600_MSA}', '', d)}"
# MSA is supported by GCC 7.1 and newer, so lets discard the feature if it is unsupported by compiler
# NOTE we can't use inline python here since external_run hasn't defined before TUNE_FEATURE is referenced
python p5600_gcc_check_msa () {
    gcc_major = int((d.getVar('GCCVERSION', True) or '0').replace('%', '').split('.')[0])
    if gcc_major >= 7:
        d.setVar('TUNE_P5600_MSA', '-mmsa')
}
p5600_gcc_check_msa[eventmask] = "bb.event.ConfigParsed"
addhandler p5600_gcc_check_msa

# Add p5600-specific code tuning
MIPS32R5_OECONF += "${@'--with-abi=32 --with-arch=mips32r5 --with-tune=p5600' if '${DEFAULTTUNE}' in ['p5600', 'p5600el', 'p5600-nf', 'p5600el-nf'] else ''}"
MIPS32R5_OECONF += "${@'--with-float=hard --with-fp-32=xx' if '${DEFAULTTUNE}' in ['p5600', 'p5600el'] else ''}"
MIPS32R5_OECONF += "${@'--with-float=soft' if '${DEFAULTTUNE}' in ['p5600-nf', 'p5600el-nf'] else ''}"
