#!/bin/bash

# host_1 ------> host_2
#                  |
#                  |
#                  v
# host_4 <------ host_3

# host_1
sudo mknod backpipe -p
nc -lvp 1234 0<backpipe | nc host_2 4321 1>backpipe

# host_2
sudo mknod backpipe -p
nc -lvp 4321 0<backpipe | nc host_3 4321 1>backpipe

# host_3
sudo mknod backpipe -p
nc -lvp 4321 0<backpipe | nc host_4 22 1>backpipe


# using host_1
ssh -p 1234 user@127.0.0.1