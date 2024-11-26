#include <iostream>
#include <vector>
#include <mutex>

using namespace std;


class Transaction {

	public:

	string sender;
	string receiver;
	int amount;

	Transaction(const string& sender, const string& receiver, int amount):sender(sender), receiver(receiver), amount(amount) {
	}
};


class Ledger {

	private:

	vector<Transaction> transactions;
	mutex ledger_mutex;

	public:

	void addTransaction(const Transaction& tx) {
		lock_guard<mutex> lock(ledger_mutex);
		transactions.push_back(tx);
	
		cout << "transaction added: " << tx.sender << " --> " << tx.receiver << ": " <<tx.amount << endl;
	}

	void displayLedger()
	{
		lock_guard<mutex> lock(ledger_mutex);
		cout << "Ledger: " << endl;
		for(const auto& tx: transactions) {
			cout << tx.sender << " --> " << tx.receiver << ": " <<tx.amount << endl;

		}
	}
};
