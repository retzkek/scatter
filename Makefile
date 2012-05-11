cc=gcc
GL=/usr/src/redhat/SOURCES/xc/extras/Mesa/include

sources=scatter.c neutrons.c explosions.c
headers=neutrons.h explosions.h frames.h texture.h

scatter: $(sources) $(headers)
	$(cc) -g -I$(GL) -lglut -o scatter $(sources)

clean:
	rm -f scatter
