#pragma once

#include "lib.h"

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
    std::random_device rd;
    std::mt19937 mt(rd());

    std::vector<size_t> idx(Transactions.size());
    std::iota(idx.begin(), idx.end(), 0);
    std::shuffle(idx.begin(), idx.end(), mt);

    std::vector<Transaction> shuffled;
    shuffled.reserve(Transactions.size());
    for (auto i : idx) shuffled.push_back(Transactions[i]);
    Transactions.swap(shuffled);

    std::vector<Transaction> tran_block;

    while (!Transactions.empty() && tran_block.size() < 100)
    {
            tran_block.push_back(Transactions.back());
            Transactions.pop_back();
    }
    
    return tran_block;
}