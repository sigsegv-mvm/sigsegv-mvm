.PHONY: all clean

all: libstrcompat.so

libstrcompat.so: strcompat.cpp strcompat.h Makefile
	g++-4.8 -std=c++11 -fno-exceptions -m32 -march=core-avx2 -mtune=core-avx2 -msse -mfpmath=sse -Wall -Wno-unused-variable -Wno-unused-function -O2 -fno-strict-aliasing -fvisibility=hidden -fvisibility-inlines-hidden -g2 -gdwarf-4 -fvar-tracking-assignments -fno-omit-frame-pointer -fuse-ld=gold -shared -fPIC -fno-gnu-unique -lbsd -o $@ $<

clean:
	-rm -f libstrcompat.so
