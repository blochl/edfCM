all: build strip

build:
	gcc -O2 edfCM.c -static -o edfCM

strip:
	strip edfCM

clean:
	rm edfCM
