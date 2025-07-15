#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>

std::string sha256(const std::string &str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(str.c_str()), str.length(), hash);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

struct Certificate {
    std::string issuer;
    std::string receiver;
    std::string content;
    time_t timestamp;

    std::string toString() const {
        return issuer + receiver + content + std::to_string(timestamp);
    }
};

class Wallet {
public:
    std::string address;
    Wallet() {
        static int counter = 0;
        address = sha256("wallet" + std::to_string(++counter));
    }
};

class Block {
public:
    int index;
    time_t timestamp;
    Certificate data;
    std::string prevHash;
    std::string hash;

    Block(int idx, const Certificate &cert, const std::string &prevHash)
        : index(idx), timestamp(time(nullptr)), data(cert), prevHash(prevHash) {
        hash = calculateHash();
    }

    std::string calculateHash() const {
        std::stringstream ss;
        ss << index << timestamp << data.toString() << prevHash;
        return sha256(ss.str());
    }
};

class Blockchain {
public:
    std::vector<Block> chain;

    Blockchain() {
        Certificate genesisCert{"Genesis", "Network", "Genesis Block", time(nullptr)};
        chain.emplace_back(0, genesisCert, "0");
    }

    void addBlock(const Certificate &cert) {
        const Block &prev = chain.back();
        chain.emplace_back(prev.index + 1, cert, prev.hash);
    }

    void printChain() const {
        for (const auto &block : chain) {
            std::cout << "Block " << block.index << "\n";
            std::cout << "Issuer: " << block.data.issuer << "\n";
            std::cout << "Receiver: " << block.data.receiver << "\n";
            std::cout << "Content: " << block.data.content << "\n";
            std::cout << "Timestamp: " << block.timestamp << "\n";
            std::cout << "Prev Hash: " << block.prevHash << "\n";
            std::cout << "Hash: " << block.hash << "\n\n";
        }
    }
};

int main() {
    Blockchain bc;
    Wallet w1, w2;

    Certificate cert1{"UniversityX", w1.address, "C++ Programming Certificate", time(nullptr)};
    Certificate cert2{"UniversityX", w2.address, "Blockchain Fundamentals", time(nullptr)};

    bc.addBlock(cert1);
    bc.addBlock(cert2);

    bc.printChain();

    return 0;
}
