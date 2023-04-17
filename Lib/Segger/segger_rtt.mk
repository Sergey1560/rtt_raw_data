C_SOURCES += \
Lib/Segger/SEGGER_RTT_printf.c \
Lib/Segger/SEGGER_RTT_Syscalls_GCC.c \
Lib/Segger/SEGGER_RTT.c

ASM_SOURCES += \
Lib/Segger/SEGGER_RTT_ASM_ARMv7M.s

C_INCLUDES += \
-ILib/Segger

