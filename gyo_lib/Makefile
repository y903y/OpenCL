main: main.cpp lib.so
	g++ -o main main.cpp ./lib.so -lOpenCL
lib.so: api.o
	g++ -shared -o lib.so api.o
api.o: api.cpp
	g++ -g -Wall -fPIC -c api.cpp -lOpenCL
clean:;	rm main *.o *.lib
