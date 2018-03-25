require conf/machine/include/tune-mips32r5.inc

# MIPS Warrior P5600 tuning feature
TUNEVALID[p5600] = "MIPS Warrior P-class P5600 Multiprocessor core"
TUNE_CCARGS .= "${@bb.utils.contains('TUNE_FEATURES', 'p5600', ' -mtune=p5600', '', d)}"

# MIPS 64-bits floating point registers feature
TUNEVALID[fp64] = "Use MIPS 64-bits FP registers"
TUNE_CCARGS .= "${@bb.utils.contains('TUNE_FEATURES', 'fp64', ' -mfp64', '', d)}"

# MIPS eXtended Physical Address (XPA) instructions
TUNEVALID[xpa] = "Use MIPS XPA instructions"
TUNE_CCARGS .= "${@bb.utils.contains('TUNE_FEATURES', 'xpa', ' -mxpa', '-mno-xpa', d)}"

# MIPS Enhanced Virtual Addressing instructions
TUNEVALID[eva] = "Use MIPS EVA instructions"
TUNE_CCARGS .= "${@bb.utils.contains('TUNE_FEATURES', 'eva', ' -meva', '-mno-eva', d)}"

# MIPS SIMD registers and instructions
TUNEVALID[eva] = "Use MIPS instructions"
TUNE_CCARGS .= "${@bb.utils.contains('TUNE_FEATURES', 'msa', ' -mmsa', '', d)}"

# MIPS Virtualization (VZ) instructions
TUNEVALID[virt] = "Use MIPS VZ instructions"
TUNE_CCARGS .= "${@bb.utils.contains('TUNE_FEATURES', 'virt', ' -mvirt', '-mno-virt', d)}"

# Append Default Tunes
TUNE_FEATURES_tune-mips32r5 += "p5600 fp64 xpa eva virt"
TUNE_FEATURES_tune-mips32r5-nf += "p5600 fp64 xpa eva virt"
TUNE_FEATURES_tune-mips32r5el += "p5600 fp64 xpa eva virt"
TUNE_FEATURES_tune-mips32r5el-nf += "p5600 fp64 xpa eva virt"

# Appended GCC default tunes
MIPS32R5_OECONF += "--with-tune=p5600 --with-fp-32=64"
MIPS32R5_NF_OECONF += "--with-tune=p5600 --with-fp-32=64"
