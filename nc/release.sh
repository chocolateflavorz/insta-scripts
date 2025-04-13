 #!/bin/bash

 #!/bin/bash

gcc -O2 -Wall -fstack-protector-strong\
  ./src/main.c \
  ./src/config.c \
  ./src/format.c \
  ./src/setui/sgwindow.c \
  ./src/setui/liawindow.c \
  ./src/commandq.c \
  ./src/con.c \
  -lncurses -lmenu -lform -lpanel -lm  \
  -o alien


strip -s --strip-unneeded -R .comment -R .gnu.version alien
upx --best alien
