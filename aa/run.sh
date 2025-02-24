 #!/bin/bash

gcc -Og -Wall -g3 --std=c2x -fsanitize=address,undefined \
  ./src/main.c \
  ./src/con.c \
  ./src/tinyexpr.c \
  -lm \
  -o invader
