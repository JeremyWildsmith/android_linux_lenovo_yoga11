cmd_arch/arm64/crypto/sha256-core.o := /home/jeremy/j706f-port/porting/yt-j706f_opensource_yt-j706f_s200107_220508_row.tar/kernel-4.14/../prebuilts/clang/host/linux-x86/clang-r383902/bin/clang -Wp,-MD,arch/arm64/crypto/.sha256-core.o.d -nostdinc -isystem /home/jeremy/j706f-port/porting/yt-j706f_opensource_yt-j706f_s200107_220508_row.tar/prebuilts/clang/host/linux-x86/clang-r383902/lib64/clang/11.0.1/include -I../arch/arm64/include -I./arch/arm64/include/generated  -I../include -I../drivers/misc/mediatek/include -I./include -I../arch/arm64/include/uapi -I./arch/arm64/include/generated/uapi -I../include/uapi -I./include/generated/uapi -include ../include/linux/kconfig.h -D__KERNEL__ -Qunused-arguments -mlittle-endian -DKASAN_SHADOW_SCALE_SHIFT=3 -D__ASSEMBLY__ --target=aarch64-linux-gnu --prefix=/home/jeremy/j706f-port/porting/yt-j706f_opensource_yt-j706f_s200107_220508_row.tar/kernel-4.14/../prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9.1/bin/ --gcc-toolchain=/home/jeremy/j706f-port/porting/yt-j706f_opensource_yt-j706f_s200107_220508_row.tar/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9.1 -no-integrated-as -Werror=unknown-warning-option -fno-PIE -DCONFIG_AS_LSE=1 -DKASAN_SHADOW_SCALE_SHIFT=3 -DCC_HAVE_ASM_GOTO -Wa,-gdwarf-2   -c -o arch/arm64/crypto/sha256-core.o arch/arm64/crypto/sha256-core.S

source_arch/arm64/crypto/sha256-core.o := arch/arm64/crypto/sha256-core.S

deps_arch/arm64/crypto/sha256-core.o := \
  ../include/linux/compiler_types.h \
    $(wildcard include/config/have/arch/compiler/h.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \

arch/arm64/crypto/sha256-core.o: $(deps_arch/arm64/crypto/sha256-core.o)

$(deps_arch/arm64/crypto/sha256-core.o):