Main:Main.cpp MD5.hpp
	g++ -o $@ Main.cpp -std=c++11 -g

.PHONY:clean
clean:
	rm Main
