cc=gcc
#lib=-framework OpenGL -framework GLUT
lib=-lGL -lglut -lGLU

sources=scatter.c neutrons.c explosions.c wm.c texture.c
headers=neutrons.h explosions.h frames.h texture.h wm.h

demo_sources=scatter-demo.c neutrons.c explosions.c
demo_headers=neutrons.h explosions.h frames.h

scatter: $(sources) $(headers)
	$(cc) -O -o scatter $(sources) $(lib)

demo: $(demo_sources) $(demo_headers)
	$(cc) -O -o scatter-demo $(demo_sources) $(lib)

clean:
	rm -f scatter
