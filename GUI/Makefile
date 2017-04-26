all: main

icons: icons.rc
	windres icons.rc -o icons.o

main: icons main.c
	gcc icons.o main.c -o main.exe -w -lgdi32 -lcomdlg32 -lws2_32 

clean:
	rm -f icons.o main.exe