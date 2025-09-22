#Copyright David-Ioan Stancu 312CAb 2023-2024
build:
	gcc -Wall -Wextra -pedantic -std=c99 image_editor.c functions.c image_properties.c -g -o image_editor -lm

pack:
	zip -FSr 312CA_StancuDavidIoan_Tema3.zip README Makefile *.c *.h

clean:
	rm -f image_editor

run: clean build

.PHONY: pack clean