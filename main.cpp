#include <iostream>
#include "hash.h"
#include <chrono>
#include <list>

class User
{
    private:

    const std::string name;
    const std::string public_key = name; // temp
    unsigned int balance = 30; // temp

    public:

    std::string get_name() {return name;}
    unsigned int get_balance() {return balance;}

    void update_balance(int update) {balance=balance+update;}
    User(std::string name) : name(name) {}


};

class Transaction
{
    private:

    const std::string sender;
    const std::string receiver;
    const unsigned int amount;

    const std::string transaction_id = Hash (sender + receiver + std::to_string(amount));

    public:
    Transaction (User& sender, User& receiver, unsigned int amount) : sender(sender.get_name()), receiver(receiver.get_name()), amount(amount) {sender.update_balance(-amount), receiver.update_balance(amount);}

    void print() {std::cout << transaction_id << ": "<< sender << " >>>> " << receiver << " | " << amount << " PatCoin\n";}
};

class Block
{
    private:

    // Header
    std::string prev_block_hash;
    const std::chrono::time_point<std::chrono::system_clock> timestamp = std::chrono::system_clock::now();
    const std::string version = "v0.1";
    unsigned int difficulty_target;

    //Body
    std::list<Transaction> Transactions;

    public:
};

int main() 
{
    User a ("Andrius");
    User b ("Paulius");

    std::cout << a.get_balance() << " " << b.get_balance() << "\n";

    Transaction c (a, b, 15);
    c.print();
    std::cout << a.get_balance() << " " << b.get_balance() << "\n";
    return 0;
}