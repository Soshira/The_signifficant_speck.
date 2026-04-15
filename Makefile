all:
	g++ main.c -o final_coordinate -lglut -lGLU -lGL && ./final_coordinate

clean: 
	rm -f app
	