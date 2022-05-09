CXX = g++ --std=c++17 -O2

all: int

int: interpreter2.o poliz2.o syntax2.o lexical2.o main.o
	${CXX} main.o interpreter2.o poliz2.o syntax2.o lexical2.o -o int

main.o: main.cpp
	${CXX} -c main.cpp

interpreter2.o: interpreter2.cpp interpreter2.h
	${CXX} -c interpreter2.cpp

poliz2.o: poliz2.cpp poliz2.h
	${CXX} -c poliz2.cpp

syntax2.o: syntax2.cpp syntax2.h
	${CXX} -c syntax2.cpp

lexical2.o: lexical2.cpp lexical2.h
	${CXX} -c lexical2.cpp

clean:
	rm *.o int
