ifeq ($(strip $(PLATFORM)),)
	PLATFORM = cell
endif

include $(CURDIR)/../Makefile-$(PLATFORM).config

TARGET		:=	libyabause
TYPE		:=	lib
INCLUDE		:=	.. ../mednafen/src src/yabause 

#Note that I just made up the "10" for VERSION
CPPFLAGS	+=	-DMDFNPS3 -DVERSION=\"10\" -DHAVE_STDINT_H -DHAVE_LIBGL

CXXFILES	+=	src/mednafen.cpp										\
				src/yabause/cd-mdfn.cpp									\
				src/yabause/sndmdfn.cpp

CFILES		+=	src/yabause/bios.c										\
				src/yabause/cs0.c										\
				src/yabause/cs1.c										\
				src/yabause/cs2.c										\
				src/yabause/error.c										\
				src/yabause/m68kcore.c									\
				src/yabause/memory.c									\
				src/yabause/netlink.c									\
				src/yabause/peripheral.c								\
				src/yabause/scsp.c										\
				src/yabause/scu.c										\
				src/yabause/sh2core.c									\
				src/yabause/sh2idle.c									\
				src/yabause/sh2int.c									\
				src/yabause/smpc.c										\
				src/yabause/thr-dummy.c									\
				src/yabause/vdp1.c										\
				src/yabause/vdp2-mdfn.c									\
				src/yabause/vidshared.c									\
				src/yabause/vidsoft.c									\
				src/yabause/yabause.c									\
				src/yabause/ygl.c										\
				src/yabause/vidogl.c									\
				src/yabause/permdfnjoy.c

#Q64 core
CFILES		+=	src/yabause/m68kq68.c									\
				src/yabause/q68/q68.c									\
				src/yabause/q68/q68-core.c
#Q68 JIT Support
#				src/yabause/q68/q68-disasm.c
#				src/yabause/q68/q68-jit.c

#C68K clashes with mednafen
#CFILES		+=	src/yabause/m68kc68k.c									\
#				src/yabause/c68k/c68k.c									\
#				src/yabause/c68k/c68kexec.c


#SH2 Dynarec
#CPPFLAGS	+=	-DSH2_DYNAREC
#CFILES		+=	src/yabause/sh2_dynarec/sh2_dynarec.c
#SFILES		+=	src/yabause/sh2_dynarec/linkage_mdfn.S




include $(CURDIR)/../Makefile-$(PLATFORM).base

