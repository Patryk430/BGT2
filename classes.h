#pragma once

#include "lib.h"

extern bool debug_mode;

class User
{
    private:

    std::string name;
    std::string public_key = name + "'s_key"; // temp
    unsigned int balance;

    public:

    std::string get_name() const {return name;}
    std::string get_key() const {return public_key;}
    unsigned int get_balance() const {return balance;}

    void print_about_me() const {std::cout << "Name: " << std::setw(25) << name << " | Balance: " << std::setw(10) << balance << " PatCoins | Public Key: " << public_key << "\n";}

    void update_balance(int update) {balance=balance+update;}
    User(std::string name, unsigned int balance) : name(name), balance(balance) {}
};

class Transaction
{
    private:

    std::string sender;
    std::string receiver;
    unsigned int amount;

    std::string transaction_id = Hash (sender + receiver + std::to_string(amount));

    public:

    Transaction (User& sender, User& receiver, unsigned int amount) : sender(sender.get_key()), receiver(receiver.get_key()), amount(amount) {}

    void print_about() {std::cout << transaction_id << ": "<< std::setw(26) << std::right << sender << " >>>> " << std::setw(26) << std::left << receiver << " | " << std::setw(5) << amount << " PatCoin\n";}
    std::string get_id() const {return transaction_id;}
    std::string get_sender() const {return sender;}
    std::string get_receiver() const {return receiver;}
    unsigned int get_amount() const {return amount;}
};

class Block
{
    private:

    //Body
    std::vector<Transaction> Transactions;

    // Header
    std::string prev_block_hash;
    long unsigned int timestamp = static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::string root_hash;
    std::string version = "v0.2";
    unsigned int nounce;
    unsigned int difficulty_target;
    unsigned int transaction_amount = Transactions.size();

    std::string block_hash = Hash (prev_block_hash + std::to_string(timestamp) + root_hash + version + std::to_string(nounce) + std::to_string(difficulty_target));



    public:

    Block (std::string prev_block_hash, unsigned int nounce, unsigned int difficulty_target, std::vector<Transaction> Transaction_Block, std::string root_hash) : prev_block_hash(prev_block_hash), nounce(nounce), difficulty_target(difficulty_target), Transactions(Transaction_Block), root_hash(root_hash) {}

    std::string get_hash() const {return block_hash;}
    const std::vector<Transaction>& get_transactions() const {return Transactions;}

    void print_info()
    {
        std::cout << "\n"
                  << "|----------------------------------------------------------------------------------|\n" <<"|         " << block_hash << "         |\n" << "|----------------------------------------------------------------------------------|\n"
                  << "| Previous Block: " << std::setw(64) << prev_block_hash    << " |\n"
                  << "| Timestamp:      " << std::setw(64) << timestamp          << " |\n"
                  << "| Root Hash:      " << std::setw(64) << root_hash          << " |\n"
                  << "| Version:        " << std::setw(64) << version            << " |\n"
                  << "| Nounce:         " << std::setw(64) << nounce             << " |\n"
                  << "| Difficulty:     " << std::setw(64) << difficulty_target  << " |\n"
                  << "| Transactions:   " << std::setw(64) << transaction_amount << " |\n"
                  << "|----------------------------------------------------------------------------------|\n" <<"|         " << block_hash << "         |\n" << "|----------------------------------------------------------------------------------|\n"
                  << "\n";

        if (debug_mode) for (Transaction t : Transactions) { t.print_about(); }
    }
};