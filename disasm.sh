#!/bin/sh
objdump -D -b binary -mi386:x86-64 -M intel "$@"