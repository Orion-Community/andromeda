
#!/bin/bash
make -C andromeda/ clean
make -C andromeda FLAGS=-D\ WARN\ -D\ MODS\ -D\ FAST\ -D\ ELFDBG CC=distcc
