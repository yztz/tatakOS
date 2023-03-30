cmake_minimum_required(VERSION 3.13)


enable_language(ASM)
set(CMAKE_OSX_DEPLOYMENT_TARGET "")

set(ARCH_DIR lib/arch/riscv)

set(ASM_DIR asm)
set(BIN_DIR bin)

# Toolchain
set(PREFIX ${ARCH}-unknown-elf-)
set(CMAKE_C_FLAGS       "-march=rv64imafc -mabi=lp64f -mcmodel=medany")
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)
set(CMAKE_ASM_COMPILER      ${PREFIX}gcc)
set(CMAKE_C_COMPILER        ${PREFIX}gcc)
set(CMAKE_CXX_COMPILER      ${PREFIX}g++)
set(CMAKE_OBJCOPY           ${PREFIX}objcopy)
set(CMAKE_OBJDUMP           ${PREFIX}objdump)
set(CMAKE_RANLIB            ${PREFIX}ranlib)
set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -fno-builtin -nostdinc -fno-stack-protector -mno-relax -ggdb -Wall")
set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} -DLAB_ID=${LAB_ID} -DLAB_DES='${LAB_DES}'")
set(CMAKE_ASM_FLAGS         ${CMAKE_C_FLAGS})
set(CMAKE_C_LINK_FLAGS      "${LINK_FLAGS} -nostdlib")

project(user)

# Library
aux_source_directory(lib LIBS)
set(LIBS ${ARCH_DIR}/crt.S lib/clone.s ${LIBS})
add_library(ulib ${LIBS} ${CMAKE_SOURCE_DIR}/lib/syscall_ids.h)
include_directories(include/)
# include_directories(../include/)
target_include_directories(ulib PRIVATE ${ARCH_DIR})


# 编译全部
aux_source_directory(src SRCS)
# 只编译要测试的文件
# set(SRCS src/openat.c ${SRCS})
# # set(SRCS src/bigwrite.c ${SRCS})
# # set(SRCS src/getfree.c ${SRCS})
# set(SRCS src/dup2.c ${SRCS})
# set(SRCS src/sleep.c ${SRCS})
# set(SRCS src/munmap.c ${SRCS})
# set(SRCS src/ls.c ${SRCS})
# set(SRCS src/waitpid.c ${SRCS})
# set(SRCS src/brk.c ${SRCS})
# set(SRCS src/mmap.c ${SRCS})
# set(SRCS src/mount.c ${SRCS})

set(EXECUTABLE_OUTPUT_PATH ${ARCH})
foreach(PATH ${SRCS})
    get_filename_component(NAME ${PATH} NAME_WE)

    add_executable(${NAME} ${PATH})
    target_link_libraries(${NAME} ulib)
    
endforeach()

add_custom_command(
        OUTPUT ${CMAKE_SOURCE_DIR}/lib/syscall_ids.h
        COMMAND sed ARGS -n -e s/__NR_/SYS_/p
        < ${CMAKE_SOURCE_DIR}/${ARCH_DIR}/syscall_ids.h.in
        > ${CMAKE_SOURCE_DIR}/lib/syscall_ids.h
        DEPENDS ${CMAKE_SOURCE_DIR}/${ARCH_DIR}/syscall_ids.h.in
)
