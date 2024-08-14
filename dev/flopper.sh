#!/bin/bash

dd if=/dev/zero of=$2 bs=512 count=2880

dd if=$1 of=$2 bs=512 seek=0 count=1 conv=notrunc
