#!/bin/bash

#                                       -------> hoct_c
#                                      /    192.168.0.2
#                                     /
#    host_a ---[ ssh ]---> host_b ---+---------> host_d
# 127.0.0.1           192.168.0.1     \     192.168.0.3
#                                      \
#                                       -------> host_e
#                                           192.168.0.4

# create socks4 proxy-server on host_a
ssh -D 127.0.0.1:2222 user@Host_B

echo "socks4 127.0.0.1 2222" >> /etc/proxychains.conf

# serch subnet (host_c, host_d, host_e)
proxychains nmap -sT -sV -v -P0 192.168.0.0/24
