globalNextTrack: globalNextTrack.c 
	clang -g -O0 -std=c11 -Wdeprecated globalNextTrack.c -o globalNextTrack `pkg-config --cflags --libs gtk+-3.0` -lX11 -lXi
	
clean:
	rm -f globalNextTrack
