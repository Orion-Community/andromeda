
#!/bin/bash
make -C andromeda/ clean -B
make -C andromeda FLAGS=-D\ MODS\ -D\ FAST CC=distcc -B
