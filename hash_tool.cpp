#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <cstdio>
#include <string>

std::string fnv(const std::string& p) {
    const uint64_t FNV_PRIME  = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 14695981039346656037ULL;
    uint64_t h = FNV_OFFSET;
    for (unsigned char c : p) { h ^= c; h *= FNV_PRIME; }
    std::stringstream ss; ss << std::hex << h; return ss.str();
}

int main() {
    // admin
    std::cout << "ADMIN:" << fnv("admin123") << "\n";
    // teachers
    const char* tpass[] = {"teach001","teach002","teach003","teach004","teach005"};
    for (auto p : tpass) std::cout << "T:" << p << ":" << fnv(p) << "\n";
    // students stu001..stu080
    for (int i = 1; i <= 80; i++) {
        char buf[16]; std::sprintf(buf, "stu%03d", i);
        std::cout << "S:" << buf << ":" << fnv(buf) << "\n";
    }
    // parents par001..par080
    for (int i = 1; i <= 80; i++) {
        char buf[16]; std::sprintf(buf, "par%03d", i);
        std::cout << "P:" << buf << ":" << fnv(buf) << "\n";
    }
    return 0;
}
