#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <string>

std::string fnv(const std::string& p) {
    const uint64_t FNV_PRIME  = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 14695981039346656037ULL;
    uint64_t h = FNV_OFFSET;
    for (unsigned char c : p) { h ^= c; h *= FNV_PRIME; }
    std::stringstream ss; ss << std::hex << h; return ss.str();
}

int main() {
    std::string pass = "admin123";
    std::string computed = fnv(pass);
    std::cout << "hash(\"admin123\") = " << computed << "\n";

    // Read stored hash from admins.csv
    std::ifstream f("data/admins.csv");
    std::string line;
    std::getline(f, line);
    std::cout << "admins.csv line: [" << line << "]\n";
    std::cout << "line length: " << line.size() << "\n";

    // Parse column 3
    int col = 0; std::string stored;
    for (char c : line) {
        if (c == ',') { col++; continue; }
        if (col == 2) stored += c;
    }
    std::cout << "stored hash:    [" << stored << "]\n";
    std::cout << "match: " << (computed == stored ? "YES" : "NO") << "\n";
    return 0;
}
