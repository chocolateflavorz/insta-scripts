 #!/bin/bash

gcc -O2 -Wall -std=c2x -fstack-protector-strong \
  ./src/main.c \
  ./src/con.c \
  ./src/tinyexpr.c \
  -lm \
  -o invader

strip -s --strip-unneeded -R .comment -R .gnu.version invader
