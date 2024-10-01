#!/bin/bash
# Setup the cross compiler enviornment

export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
