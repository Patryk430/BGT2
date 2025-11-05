#include "lib.h"

std::atomic<bool> stop_mining(false);

Block Mine_Block_With_Timeout(const std::string& prev_block_hash, unsigned int difficulty_target, std::vector<Transaction> transactions, int timeout_seconds)
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

void Remove_Mined_Transactions(std::vector<Transaction>& pool, const std::vector<Transaction>& mined)
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

void PrintTopRichestUsers(std::vector<User> sorted_users, int top_n = 10) 
{
    std::sort(sorted_users.begin(), sorted_users.end(),
              [](User& a, User& b) { return a.get_balance() > b.get_balance(); });

    std::cout << "Top " << top_n << " richest users:\n";
    for (int i = 0; i < top_n && i < (int)sorted_users.size(); ++i) {
        User& u = sorted_users[i];
        std::cout << i + 1 << ". " << u.get_name() << " - Balance: " << u.get_balance() << "\n";
    }
    std::cout << std::endl;
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

    std::cout << "| RICHEST USERS BEFORE MINING |\n";
    PrintTopRichestUsers(Users, 10);

    std::cout << "| COMMENSING MINING |\n";

    while (!Transactions.empty())
    {
        std::vector<Transaction> T_Block = Generate_Transaction_Block(Transactions);

        int timeout_seconds = 5;
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

std::cout << "| RICHEST USERS AFTER MINING |\n";
PrintTopRichestUsers(Users, 10);

return 0;
}
