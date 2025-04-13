 #!/bin/bash


gcc -Og -Wall \
  ./src/main.c ./src/menu_window.c ./src/message_dialog.c ./src/form_dialog.c \
  ./src/ops.c ./src/loadconf.c \
  -lncurses -lmenu -lform \
  -o alien



