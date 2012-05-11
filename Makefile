cc=gcc
lib=-framework OpenGL -framework GLUT
#GL=/usr/src/redhat/SOURCES/xc/extras/Mesa/include

sources=scatter.c neutrons.c explosions.c wm.c texture.c
headers=neutrons.h explosions.h frames.h texture.h wm.h

scatter: $(sources) $(headers)
	$(cc) -O $(lib) -o scatter $(sources)

clean:
	rm -f scatter
