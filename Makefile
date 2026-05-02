all:
	g++ main.cpp -o final_coordinate -lglut -lGLU -lGL && ./final_coordinate

clean: 
	rm -f app
	