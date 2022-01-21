# Project: IZP Projekt 1
# Autor:   Evgenii Shiliaev
# Datum:   21.01.2022

CC=gcc
CFLAGS= -std=c99 -Wall -Wextra -Werror -g

sheet: sheet.c
        $(CC) $(CFLAGS) sheet.c -o sheet

# End of Makefile
