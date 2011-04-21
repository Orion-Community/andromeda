#!/bin/bash

# Copy first sector

dd if=loader.bin of=goldeneaglebl.bin seek=0

# Copy second sector

dd if=stage2.bin of=goldeneaglebl.bin seek=1
