chess: chess.cpp
	g++ -Wall -o chess chess.cpp
clean:
	rm -f chess core
run:
	./chess