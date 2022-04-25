#!/bin/bash

ls -lstra /dev/shm
./init_list
./leader_test
ls -lstra /dev/shm
./cleanup_list
rm /dev/shm/region4
ls -lstra /dev/shm
