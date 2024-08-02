all:
	g++ -o bin/a.out -std=c++17 util/Channel.cpp procedural/main.cpp procedural/types.cpp

move:
	g++ -o bin/move -std=c++17 move_semantics.cpp

format:
	find . -iname '*.h' -o -iname '*.cpp' | xargs clang-format -style=file -i