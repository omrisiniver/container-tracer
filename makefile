
build_trace:
	g++ -std=c++17 -pthread DispatchQueue.cpp -lstdc++fs -o test

build_bin:
	g++ -std=c++17 Bin.cpp -o testing