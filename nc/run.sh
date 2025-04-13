 #!/bin/bash

gcc -Og -Wall -g3 -fsanitize=address,undefined -D DEBUG \
  ./src/main.c \
  ./src/ctl/config.c \
  ./src/ctl/configloader.c \
  ./src/ui/format.c \
  ./src/ui/common.c \
  ./src/ui/sgwindow.c \
  ./src/ui/liawindow.c \
  ./src/ctl/commandq.c \
  ./src/ctl/con.c \
  -lncurses -lmenu -lform -lpanel -lm \
  -o alien
