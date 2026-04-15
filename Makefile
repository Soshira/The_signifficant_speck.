all:
	g++ just_a_dot.cpp -o final_coordinate -lglut -lGLU -lGL && ./final_coordinate

clean: 
	rm -f app
	