#pragma once
#include <string>
#include <sstream>
#include <iomanip>

/// Security utility for password hashing and verification
class Crypto {
public:
    /// FNV-1a hash function for simple password hashing (non-cryptographic but prevents plain text)
    static std::string hashPassword(const std::string& password) {
        if (password.empty()) return "";
        
        const uint64_t FNV_PRIME = 1099511628211ULL;
        const uint64_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
        
        uint64_t hash = FNV_OFFSET_BASIS;
        for (unsigned char c : password) {
            hash ^= c;
            hash *= FNV_PRIME;
        }
        
        // Convert to hex string
        std::stringstream ss;
        ss << std::hex << hash;
        return ss.str();
    }
    
    /// Verify password by hashing and comparing
    static bool verifyPassword(const std::string& inputPassword, const std::string& storedHash) {
        return hashPassword(inputPassword) == storedHash;
    }
};
