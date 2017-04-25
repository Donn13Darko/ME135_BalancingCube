all: main

icons: icons.rc
	windres icons.rc -o icons.o

main: icons main.c
	gcc icons.o main.c -o main.exe -lgdi32 -lcomdlg32 -lws2_32 -w

clean:
	rm -f icons.o main.exe