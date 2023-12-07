#include <iostream>
#include <unordered_map>
#include <stdexcept>

class InMemoryDB
{
private:
    std::unordered_map<std::string, int> data;            // represents actual data in DB
    std::unordered_map<std::string, int> transactionData; // temp storage for transaction
    bool inTransaction;

public:
    InMemoryDB() : inTransaction(false) {}

    int get(const std::string &key)
    {
        if (!inTransaction && data.count(key)) // only provides val if not in transaction and if the key is in the map
        {
            return data[key];
        }
        return -1; // Return -1 if key doesn't exist or in trans
    }

    void put(const std::string &key, int val)
    {
        if (!inTransaction)
        {
            throw std::logic_error("Transaction not in progress");
        }
        transactionData[key] = val; // add the val to the temp storage
    }

    void begin_transaction()
    {
        if (inTransaction)
        {
            throw std::logic_error("Transaction already in progress");
        }
        inTransaction = true;
        transactionData.clear();
        transactionData.insert(data.begin(), data.end()); // Copy current data to transaction data
    }

    void commit()
    {
        if (!inTransaction)
        {
            throw std::logic_error("No transaction in progress");
        }
        data.insert(transactionData.begin(), transactionData.end()); // add chenges made in transaction to data
        endTransaction();
    }

    void rollback()
    {
        if (!inTransaction)
        {
            throw std::logic_error("No transaction in progress");
        }
        endTransaction();
    }

private:
    void endTransaction()
    {
        inTransaction = false;   // end transaction
        transactionData.clear(); // Clear all the transaction data when rolled back
    }
};

int main()
{
    InMemoryDB inmemoryDB;

    // should return null, because A doesn’t exist in the DB yet
    std::cout << inmemoryDB.get("A") << std::endl;

    // should throw an error because a transaction is not in progress
    try
    {
        inmemoryDB.put("A", 5); // Transaction not in progress
    }
    catch (const std::logic_error &e)
    {
        std::cerr << e.what() << std::endl;
    }

    // starts a new transaction
    inmemoryDB.begin_transaction();

    // set’s value of A to 5, but its not committed yet
    inmemoryDB.put("A", 5);

    // should return null, because updates to A are not committed yet
    std::cout << inmemoryDB.get("A") << std::endl;

    // update A’s value to 6 within the transaction
    inmemoryDB.put("A", 6);

    // commits the open transaction
    inmemoryDB.commit();

    // should return 6, that was the last value of A to be committed
    std::cout << inmemoryDB.get("A") << std::endl;

    // throws an error, because there is no open transaction
    try
    {
        inmemoryDB.commit(); // No transaction in progress
    }
    catch (const std::logic_error &e)
    {
        std::cerr << e.what() << std::endl;
    }

    // throws an error because there is no ongoing transaction
    try
    {
        inmemoryDB.rollback(); // No transaction in progress
    }
    catch (const std::logic_error &e)
    {
        std::cerr << e.what() << std::endl;
    }

    // should return null because B does not exist in the database
    std::cout << inmemoryDB.get("B") << std::endl;

    // starts a new transaction
    inmemoryDB.begin_transaction();

    // Set key B’s value to 10 within the transaction
    inmemoryDB.put("B", 10);

    // Rollback the transaction - revert any changes made to B
    inmemoryDB.rollback();

    // Should return null because changes to B were rolled back
    std::cout << inmemoryDB.get("B") << std::endl;

    return 0;
}
