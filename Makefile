main.out: main.cpp Arrow.o Fluid.o Tunnel.o misc.o
	g++ main.cpp Arrow.o Fluid.o Tunnel.o misc.o -o main.out -lsfml-graphics -lsfml-window -lsfml-system -pthread -std=c++11
# for profiling add -g and -pg. Then run gprof main.out gmon.out > profile.txt. Dont forget to return 0 in main function

Arrow.o: Arrow.cpp Arrow.h
	g++ -c Arrow.cpp -lsfml-graphics -lsfml-window -lsfml-system -std=c++11

Fluid.o: Fluid.cpp Fluid.h
	g++ -c Fluid.cpp -lsfml-graphics -lsfml-window -lsfml-system -std=c++11

Tunnel.o: Tunnel.cpp Tunnel.h
	g++ -c Tunnel.cpp -lsfml-graphics -lsfml-window -lsfml-system -std=c++11

misc.o: misc.cpp misc.h
	g++ -c misc.cpp
