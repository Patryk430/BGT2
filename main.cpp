#include <iostream>
#include "hash.h"
#include "realistic_names.cpp"
#include <chrono>
#include <list>
#include <iomanip>
#include <algorithm>

bool debug_mode = false;

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
     const std::vector<Transaction>& get_transactions() const {
    return Transactions;
}

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

// Compute the Merkle Root from a list of transaction IDs
std::string Compute_Merkle_Root(const std::vector<Transaction>& transactions)
{
    if (transactions.empty()) 
        return Hash("EMPTY_BLOCK");  // handle edge case

    // Step 1: Collect all transaction hashes
    std::vector<std::string> layer;
    for (auto tx : transactions)
        layer.push_back(tx.get_id());

    // Step 2: Build up the tree until one hash remains
    while (layer.size() > 1)
    {
        // If odd number of elements, duplicate the last one (Bitcoin-style)
        if (layer.size() % 2 != 0)
            layer.push_back(layer.back());

        std::vector<std::string> next_layer;
        for (size_t i = 0; i < layer.size(); i += 2)
        {
            std::string combined = layer[i] + layer[i + 1];
            next_layer.push_back(Hash(combined));
        }
        layer = next_layer;
    }

    // Step 3: Final hash is the Merkle Root
    return layer[0];
}

struct MerkleNode
{
    std::string sibling_hash;
    bool sibling_is_left;
};

std::vector<MerkleNode> Generate_Merkle_Proof(const std::vector<Transaction>& transactions, const std::string& target_txid)
{
    std::vector<std::string> layer;
    for (auto tx : transactions)
        layer.push_back(tx.get_id());

    std::vector<MerkleNode> proof;
    std::string current_hash = target_txid;

    while (layer.size() > 1)
    {
        if (layer.size() % 2 != 0)
            layer.push_back(layer.back());

        std::vector<std::string> next_layer;

        for (size_t i = 0; i < layer.size(); i += 2)
        {
            std::string left = layer[i];
            std::string right = layer[i + 1];
            std::string parent = Hash(left + right);

            if (left == current_hash)
            {
                proof.push_back({right, false}); // sibling on the right
                current_hash = parent;
            }
            else if (right == current_hash)
            {
                proof.push_back({left, true}); // sibling on the left
                current_hash = parent;
            }

            next_layer.push_back(parent);
        }

        layer = next_layer;
    }

    return proof;
}

bool Verify_Merkle_Proof(std::string txid, const std::vector<MerkleNode>& proof, const std::string& root)
{
    std::string hash = txid;

    for (const auto& p : proof)
    {
        if (p.sibling_is_left)
            hash = Hash(p.sibling_hash + hash);
        else
            hash = Hash(hash + p.sibling_hash);
    }

    return hash == root;
}


Block Mine_Block (std::string prev_block_hash, unsigned int difficulty_target, std::vector<Transaction> transaction_block)
{
    int nounce = 0;
    std::string difficulty(difficulty_target, '0');

    std::string merkle_root = Compute_Merkle_Root(transaction_block);

    while (true)
    {
        Block new_block(prev_block_hash, nounce++, difficulty_target, transaction_block, merkle_root);
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

void Remove_Mined_Transactions(std::vector<Transaction>& pool,
                               const std::vector<Transaction>& mined)
{
    pool.erase(
        std::remove_if(pool.begin(), pool.end(),
            [&](const Transaction& tx) {
                return std::any_of(mined.begin(), mined.end(),
                    [&](const Transaction& mt) {
                        return tx.get_id() == mt.get_id();
                    });
            }),
        pool.end());
}

bool Verify_Transaction(User& sender, User& receiver, unsigned int amount) 
{
    if (sender.get_key() == receiver.get_key()) return false;
    if (amount == 0) return false;
    if (sender.get_balance() < amount) return false;
    return true;
}

#include <thread>
#include <atomic>
#include <vector>
#include <optional>
#include <chrono>

// Thread-safe global mining flag
std::atomic<bool> stop_mining(false);

Block Mine_Block_With_Timeout(
    const std::string& prev_block_hash,
    unsigned int difficulty_target,
    std::vector<Transaction> transactions,
    int timeout_seconds)
{
    std::optional<Block> result;
    unsigned int start_nonce = 0; // Continue nonce across retries

    while (!result.has_value()) // Keep trying until a block is mined
    {
        stop_mining = false;
        std::vector<std::thread> miners;
        auto start_time = std::chrono::steady_clock::now();

        // Launch 5 miners
        for (int i = 0; i < 5; ++i)
        {
            miners.emplace_back([&, i]() {
                try {
                    std::string difficulty(difficulty_target, '0');
                    std::string merkle_root = Compute_Merkle_Root(transactions);
                    unsigned int nonce = start_nonce;

                    while (!stop_mining)
                    {
                        Block candidate(prev_block_hash, nonce++, difficulty_target, transactions, merkle_root);
                        auto hash = candidate.get_hash();

                        if (hash.substr(0, difficulty_target) == difficulty)
                        {
                            if (!stop_mining.exchange(true)) // first thread to win
                            {
                                result = candidate;
                                std::cout << "✅ Block mined by miner " << (i + 1) << "!\n";
                            }
                            return;
                        }

                        // Timeout check
                        auto now = std::chrono::steady_clock::now();
                        if (std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count() >= timeout_seconds)
                        {
                            stop_mining = true;
                            return;
                        }
                    }
                }
                catch (...) {}
            });
        }

        // Join all miners safely
        for (auto& m : miners)
            if (m.joinable()) m.join();

        if (!result.has_value())
        {
            // Update start_nonce to continue from last attempted value
            start_nonce += 1000000; // arbitrary large increment to avoid retrying same nonces
            std::cout << "⏳ Timeout reached — retrying mining...\n";
        }
    }

    return *result;
}



int main() 
{
    int difficulty = 3;
      
    char input;
    std::cout << "D for debug: "; std::cin >> input; 
    if (input == 'D') debug_mode = 1;
    std::cout << "\n\n";

    std::cout << "| GENERATING USERS |\n";
    std::vector <User> Users = Generate_Users(100);
    if (debug_mode) for (User u : Users) { u.print_about_me(); }
    std::cout << "| USERS GENERATED  |\n\n";

    std::cout << "| GENERATING TRANSACTIONS |\n";
    std::vector<Transaction> Transactions = Generate_Transactions(Users, 366);
    if (debug_mode) for (Transaction t : Transactions) { t.print_about(); }
    std::cout << "| TRANSACTIONS GENERATED  |\n\n";

    std::list<Block> Blockchain;

    std::cout << "| COMMENSING MINING |\n";

    while (!Transactions.empty())
    {
        std::vector<Transaction> T_Block = Generate_Transaction_Block(Transactions);

        int timeout_seconds = 1;
        std::string prev_hash = Blockchain.empty()
            ? std::string(64, '0')
            : Blockchain.back().get_hash();

        std::optional<Block> mined_block = Mine_Block_With_Timeout(prev_hash, difficulty, T_Block, timeout_seconds);

        if (!mined_block)
        {
            std::cout << "⚠️ No block mined within timeout.\n";
            break;
        }

        // Add block to chain
        Blockchain.push_back(*mined_block);

        // Remove mined transactions from the pool
        Remove_Mined_Transactions(Transactions, mined_block->get_transactions());

        // Merkle root and proof for verification
        std::string merkle_root = Compute_Merkle_Root(mined_block->get_transactions());
        std::string target_txid = mined_block->get_transactions().front().get_id();
        auto proof = Generate_Merkle_Proof(mined_block->get_transactions(), target_txid);

        if (Verify_Merkle_Proof(target_txid, proof, merkle_root))
            std::cout << "✅ Transaction verified via Merkle proof.\n";
        else
            std::cout << "❌ Transaction verification failed!\n";

        // Update user balances
        for (auto& t : mined_block->get_transactions())
        {
            for (auto& u : Users)
            {
                if (u.get_key() == t.get_sender() && u.get_balance() >= t.get_amount())
                    u.update_balance(-t.get_amount());
                if (u.get_key() == t.get_receiver())
                    u.update_balance(t.get_amount());
            }
        }
    }



    std::cout << "| MINING COMPLETE |\n";

    for (Block b : Blockchain)
    {
        b.print_info();
    }

    /*for (User u : Users)
    {
        u.print_about_me();
    }*/
    return 0;
}
