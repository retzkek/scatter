cc=gcc
lib=-framework OpenGL -framework GLUT
#GL=/usr/src/redhat/SOURCES/xc/extras/Mesa/include

sources=scatter.c neutrons.c explosions.c wm.c texture.c
headers=neutrons.h explosions.h frames.h texture.h wm.h

demo_sources=scatter-demo.c neutrons.c explosions.c
demo_headers=neutrons.h explosions.h frames.h

scatter: $(sources) $(headers)
	$(cc) -O $(lib) -o scatter $(sources)

demo: $(demo_sources) $(demo_headers)
	$(cc) -O $(lib) -o scatter-demo $(demo_sources)

clean:
	rm -f scatter
