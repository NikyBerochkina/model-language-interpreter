CXX = g++ --std=c++17 -O2
# CXX = g++ --std=c++17 -g

all: int lexical poliz debug

int: interpreter2.o poliz2.o syntax2.o lexical2.o main.o
	${CXX} main.o interpreter2.o poliz2.o syntax2.o lexical2.o -o int

lexical: interpreter2.o poliz2.o syntax2.o lexical2.o lexical_main.o
	${CXX} lexical_main.o interpreter2.o poliz2.o syntax2.o lexical2.o -o lexical

poliz: poliz_main.o poliz2.o syntax2.o lexical2.o lexical_main.o
	${CXX} poliz_main.o interpreter2.o poliz2.o syntax2.o lexical2.o -o poliz

debug: interpreter2.o poliz2.o syntax2.o lexical2.o debug_main.o
	${CXX} debug_main.o interpreter2.o poliz2.o syntax2.o lexical2.o -o debug

lexical_main.o: main.cpp
	${CXX} -c main.cpp -DLEXICAL -o lexical_main.o

poliz_main.o: main.cpp
	${CXX} -c main.cpp -DPOLIZ -o poliz_main.o

debug_main.o: main.cpp
	${CXX} -c main.cpp -DDEBUG_INTERPRETER=1 -o debug_main.o

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
	rm *.o int lexical poliz debug
