#!/bin/bash
# echo latency measurements
# lmbench_all lat_syscall -P 1 null # 6
# lmbench_all lat_syscall -P 1 read # 26
# lmbench_all lat_syscall -P 1 write # 11
# busybox mkdir -p /var/tmp
# busybox touch /var/tmp/lmbench
# lmbench_all lat_syscall -P 1 stat /var/tmp/lmbench # 1100
# lmbench_all lat_syscall -P 1 fstat /var/tmp/lmbench # 111
# lmbench_all lat_syscall -P 1 open /var/tmp/lmbench # 1200
# lmbench_all lat_select -n 100 -P 1 file # 250
# lmbench_all lat_sig -P 1 install # 35
# lmbench_all lat_sig -P 1 catch # 70
# lmbench_all lat_sig -P 1 prot lat_sig # 12
# # lmbench_all lat_pipe -P 1 # 1113
# lmbench_all lat_proc -P 1 fork # 5000
# lmbench_all lat_proc -P 1 exec # 49992
# busybox cp hello /tmp
# lmbench_all lat_proc -P 1 shell # 1019992 page recycle
# lmbench_all lmdd label="File /var/tmp/XXX write bandwidth:" of=/var/tmp/XXX move=1m fsync=1 print=3 # 2330 KB/sec
# lmbench_all lat_pagefault -P 1 /var/tmp/XXX # unknown sys call 227
# lmbench_all lat_mmap -P 1 512k /var/tmp/XXX # 0.524288 3749
# busybox echo file system latency
#                             # 0k      1       100     100
# lmbench_all lat_fs /var/tmp # 1k      1       33      50
#                             # 4k      1       33      100
#                             # 10k     1       25      50
# busybox echo Bandwidth measurements
# # lmbench_all bw_pipe -P 1 # 36.63
# lmbench_all bw_file_rd -P 1 512k io_only /var/tmp/XXX # 0.524288 83.9
# lmbench_all bw_file_rd -P 1 512k open2close /var/tmp/XXX # 0.524288, 4.03
# lmbench_all bw_mmap_rd -P 1 512k mmap_only /var/tmp/XXX # 0.524288 7974.76
# lmbench_all bw_mmap_rd -P 1 512k open2close /var/tmp/XXX # 0.524288 0.476628
# busybox echo context switch overhead
lmbench_all lat_ctx -P 1 -s 32 2 4 8 16 24 32 64 96 # no result?
