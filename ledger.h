#ifndef LEDGER_H
#define LEDGER_H

#include <string>
#include <vector>
#include <mutex>

using namespace std;

class Transaction {
public:
	string sender;
	string receiver;
	int amount;

	Transaction(const string& sender, const string& receiver, int amount);
};

class Ledger {
private:
	vector<Transaction> transactions;
	mutex ledger_mutex;

public:
	void addTransaction(const Transaction& tx);
	void displayLedger();
};
#endif
