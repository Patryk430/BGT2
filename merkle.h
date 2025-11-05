#pragma once

#include "lib.h"


struct MerkleNode
{
    std::string sibling_hash;
    bool sibling_is_left;
};

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