all:
	g++ -pthread -o node node.cpp ledger.cpp
	g++ -pthread -o wallet_ap wallet_ap.cpp 
clean:
	rm node wallet_ap
