#include <iostream>
#include "hash.h"
#include "realistic_names.cpp"
#include <chrono>
#include <list>
#include <iomanip>
#include <algorithm>

class User
{
    private:

    const std::string name;
    const std::string public_key = name + "'s_key"; // temp
    unsigned int balance;

    public:

    const std::string get_name() {return name;}
    const std::string get_key() {return public_key;}
    const unsigned int get_balance() {return balance;}

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
    Transaction (User& sender, User& receiver, unsigned int amount) : sender(sender.get_key()), receiver(receiver.get_key()), amount(amount) {}

    void print_about() {std::cout << transaction_id << ": "<< std::setw(26) << std::right << sender << " >>>> " << std::setw(26) << std::left << receiver << " | " << std::setw(5) << amount << " PatCoin\n";}
    const std::string get_id() {return transaction_id;}
    const std::string get_sender() {return sender;}
    const std::string get_receiver() {return receiver;}
    const unsigned int get_amount() {return amount;}
};

class Block
{
    private:

    //Body
    const std::vector<Transaction> Transactions;

    /*std::string all_transaction_id ()
    {
        std::string temp = "";
        for (Transaction t : Transactions)
        {
            temp += t.get_id();    
        }
        return temp;
    }
    */
  

    // Header
    const std::string prev_block_hash;
    const long unsigned int timestamp = static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    const std::string root_hash;
    const std::string version = "v0.1";
    const unsigned int nounce;
    const unsigned int difficulty_target;

    const std::string block_hash = Hash (prev_block_hash + std::to_string(timestamp) + root_hash + version + std::to_string(nounce) + std::to_string(difficulty_target));



    public:

    Block (std::string prev_block_hash, unsigned int nounce, unsigned int difficulty_target, std::vector<Transaction> Transaction_Block, std::string root_hash) : prev_block_hash(prev_block_hash), nounce(nounce), difficulty_target(difficulty_target), Transactions(Transaction_Block), root_hash(root_hash) {}

    const std::string get_hash() {return block_hash;}

    void print_info()
    {
        std::cout << "\n"
                  << "|----------------------------------------------------------------------------------|\n" <<"|         " << block_hash << "         |\n" << "|----------------------------------------------------------------------------------|\n"
                  << "| Previous Block: " << std::setw(64) << prev_block_hash   << " |\n"
                  << "| Timestamp:      " << std::setw(64) << timestamp         << " |\n"
                  << "| Root Hash:      " << std::setw(64) << root_hash         << " |\n"
                  << "| Version:        " << std::setw(64) << version           << " |\n"
                  << "| Nounce:         " << std::setw(64) << nounce            << " |\n"
                  << "| Difficulty:     " << std::setw(64) << difficulty_target << " |\n"
                  << "|----------------------------------------------------------------------------------|\n" <<"|         " << block_hash << "         |\n" << "|----------------------------------------------------------------------------------|\n"
                  << "\n";

//        for (Transaction t : Transactions)
//        {
//            t.print_about();
//        }
    }
};

std::vector<User> Generate_Users(int amount) 
{
    std::vector<User> Users;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(100, 1000000);
    
    for (int i = 0; i < amount; i++)
    {
        Users.push_back (User (names[i], distr(gen)));
    }

    return Users;
} 

std::vector<Transaction> Generate_Transactions(std::vector<User>& Users, int amount) 
{
    std::vector<Transaction> Transactions;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(1, 1000);
    std::uniform_int_distribution<> user_distr(0, Users.size()-1);

    for (int i = 0; i < amount; i++)
    {
        Transactions.push_back (Transaction (Users.at(user_distr(gen)), Users.at(user_distr(gen)), distr(gen)));
    }

    return Transactions;
}

std::vector<Transaction> Generate_Transaction_Block(std::vector<Transaction>& Transactions)
{
    std::vector<Transaction> tran_block;

    while (!Transactions.empty() && tran_block.size() < 50)
    {
            tran_block.push_back(Transactions.back());
            Transactions.pop_back();
    }
    
    return tran_block;
}

Block Mine_Block (std::string prev_block_hash, unsigned int difficulty_target, std::vector<Transaction> transaction_block)
{
    int nounce = 0;
    std::string difficulty(difficulty_target, '0');

        std::string temp = "";
        for (Transaction t : transaction_block)
        {
            temp += t.get_id();    
        }

    while (true)
    {
        Block new_block(prev_block_hash, nounce++, difficulty_target, transaction_block, Hash(temp));
        std::string new_block_hash = new_block.get_hash();
        //std::cout << new_block_hash << std::endl;

        if (new_block_hash.substr(0, difficulty_target) == difficulty)
        {
            std::cout << "\n|-------------|\n|             |\n| BLOCK MINED |\n|             |\n|-------------|\n\n";
            return new_block;
            break;
        }
    }
}

int main() 
{
    int difficulty = 3;
    std::random_device rd;
    std::vector <User> Users = Generate_Users(2);

    for (User u : Users)
    {
        u.print_about_me();
    }

    std::vector<Transaction> Transactions = Generate_Transactions(Users, 150);

    //std::shuffle(Transactions.begin(), Transactions.end(), rd);

    for (Transaction t : Transactions)
    {
        t.print_about();
    }

    std::list<Block> Blockchain;

    while (!Transactions.empty())
    {
        std::vector<Transaction> T_Block  = Generate_Transaction_Block(Transactions);

        if (Blockchain.empty()) Blockchain.push_back(Mine_Block("0000000000000000000000000000000000000000000000000000000000000000", difficulty, T_Block));
        else Blockchain.push_back(Mine_Block(Blockchain.back().get_hash(), difficulty, T_Block));

        for (int i = 0; i < T_Block.size(); i++) 
        {
            for (int j = 0; j < Users.size(); j++)
            {
                if (Users[j].get_key() == T_Block[i].get_sender()) Users[j].update_balance(-T_Block[i].get_amount());
                if (Users[j].get_key() == T_Block[i].get_receiver()) Users[j].update_balance(T_Block[i].get_amount());
            }
        }
    }

    for (Block b : Blockchain)
    {
        b.print_info();
    }

    for (User u : Users)
    {
        u.print_about_me();
    }
    return 0;
}
