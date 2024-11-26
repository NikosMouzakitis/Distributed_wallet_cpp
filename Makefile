all:
	g++ -pthread -o node node.cpp ledger.cpp
clean:
	rm node
