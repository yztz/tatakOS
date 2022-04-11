#!/bin/sh
make run platform=qemu EXTRA_QEMUOPTS='-S -gdb tcp::1234' CPUS=1