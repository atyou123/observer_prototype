all:
	g++ -o bin/a.out -std=c++17 util/Channel.cpp procedural/main.cpp util/Serialization.cpp

format:
	find . -iname '*.h' -o -iname '*.cpp' | xargs clang-format -style=file -i