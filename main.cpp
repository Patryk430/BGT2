#include <iostream>
#include "hash.h"
#include "realistic_names.cpp"
#include <chrono>
#include <list>
#include <iomanip>

class User
{
    private:

    const std::string name;
    const std::string public_key = name + "'s_key"; // temp
    unsigned int balance = 30; // temp

    public:

    std::string get_name() {return name;}
    std::string get_key() {return public_key;}
    unsigned int get_balance() {return balance;}

    void print_about_me() {std::cout << "Name: " << std::setw(25) << name << " | Balance: " << std::setw(10) << balance << " PatCoins | Public Key: " << public_key << "\n";}

    void update_balance(int update) {balance=balance+update;}
    User(std::string name, unsigned int balance) : name(name), balance(balance) {}


};

class Transaction
{
    private:

    const std::string sender;
    const std::string receiver;
    const unsigned int amount;

    const std::string transaction_id = Hash (sender + receiver + std::to_string(amount));

    public:
    Transaction (User& sender, User& receiver, unsigned int amount) : sender(sender.get_key()), receiver(receiver.get_key()), amount(amount) {sender.update_balance(-amount), receiver.update_balance(amount);}

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

std::vector<User> Generate_Users() 
{
    std::vector<User> Users;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(100, 1000000);
    
    for (int i = 0; i < 1000; i++)
    {
        Users.push_back (User (names[i], distr(gen)));
    }

    return Users;
} 

int main() 
{
    std::vector <User> Users = Generate_Users();

    for (User u : Users)
    {
        u.print_about_me();
    }

    User a ("Andrius", 100);
    User b ("Paulius", 40);

    std::cout << a.get_balance() << " " << b.get_balance() << "\n";

    Transaction c (a, b, 15);
    c.print();
    std::cout << a.get_balance() << " " << b.get_balance() << "\n";
    return 0;
}