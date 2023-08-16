build:
	g++ -g filecompress.cpp encodingTree.cpp -O3 -o main.exe

run:
	./main.exe

gtests:
	g++ -std=c++14 tests.cpp encodingTree.cpp -o tests.exe -lgtest -lgtest_main -lpthread -g

tests:
	./tests.exe
