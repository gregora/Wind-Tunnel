CUDA_PATH = /usr/local/cuda
all: main.out mainCUDA.out

main.out: main.cpp Arrow.o Fluid.o Tunnel.o misc.o
	g++ main.cpp Arrow.o Fluid.o Tunnel.o misc.o -o main.out -lsfml-graphics -lsfml-window -lsfml-system -pthread -std=c++11
# for profiling add -g and -pg. Then run gprof main.out gmon.out > profile.txt. Dont forget to return 0 in main function

mainCUDA.out: main.cpp Arrow.o FluidCUDA.o TunnelCUDA.o misc.o
	g++ main.cpp Arrow.o FluidCUDA.o TunnelCUDA.o misc.o -o mainCUDA.out -lsfml-graphics -lsfml-window -lsfml-system -pthread -L$(CUDA_PATH)/lib64 -lcudart -std=c++11

Arrow.o: Arrow.cpp Arrow.h
	g++ -c Arrow.cpp -lsfml-graphics -lsfml-window -lsfml-system -std=c++11

Fluid.o: Fluid.cpp Fluid.h
	g++ -c Fluid.cpp -lsfml-graphics -lsfml-window -lsfml-system -std=c++11

FluidCUDA.o: FluidCUDA.cu Fluid.h
	nvcc -c FluidCUDA.cu -std=c++11

Tunnel.o: Tunnel.cpp Tunnel.h
	g++ -c Tunnel.cpp -lsfml-graphics -lsfml-window -lsfml-system -std=c++11

TunnelCUDA.o: TunnelCUDA.cu Tunnel.h
	nvcc -c TunnelCUDA.cu -std=c++11

misc.o: misc.cpp misc.h
	g++ -c misc.cpp
