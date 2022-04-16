#include <cstdint>
#include <queue>
#include <unordered_map>
#include <list>
#include "huffmanCode.h"

namespace xorCode {

    Leaf::Leaf(uint8_t byte) {
        this->byte = byte;
    }

    void Leaf::fillMap(std::unordered_map<uint8_t, std::string> *map, std::list<char> *curSeq) const {
        std::string value;
        for (auto c: *curSeq) {
            value.append(sizeof(char), c);
        }
        (*map)[byte] = value;
    }

    uint8_t Leaf::decode(const std::string &seq, int *pos) const {
        return byte;
    }

    Composite::Composite(TreeNode *left, TreeNode *middle, TreeNode *right) {
        this->left = left;
        this->middle = middle;
        this->right = right;
    }

    void Composite::fillMap(std::unordered_map<uint8_t, std::string> *map, std::list<char> *curSeq) const {
        curSeq->push_back('0');
        left->fillMap(map, curSeq);
        curSeq->pop_back();
        curSeq->push_back('1');
        middle->fillMap(map, curSeq);
        curSeq->pop_back();
        curSeq->push_back('2');
        right->fillMap(map, curSeq);
        curSeq->pop_back();
    }

    uint8_t Composite::decode(const std::string &seq, int *pos) const {
        if (*pos >= seq.length()) {
            ++(*pos);
            return 0;
        }
        switch (seq.at(*pos)) {
            case '0':
                ++(*pos);
                return left->decode(seq, pos);
            case '1':
                ++(*pos);
                return middle->decode(seq, pos);
            case '2':
                ++(*pos);
                return right->decode(seq, pos);
            default:
                ++(*pos);
                return left->decode(seq, pos);
        }
    }

    HuffmanCode *HuffmanCode::instance = nullptr;

    HuffmanCode *HuffmanCode::getInstance() {
        if (instance == nullptr) {
            instance = new HuffmanCode();
        }
        return instance;
    }

    HuffmanCode::HuffmanCode() {
        std::queue<TreeNode *> q;
        // create a node for each possible byte
        uint8_t byte = 0;
        for (int i = 0; i < 256; ++i) {
            q.push(new Leaf(byte));
            ++byte;
        }
        // add an extra node so that every node later on will have exactly three children
        q.push(new Leaf(255));
        while (q.size() > 1) {
            auto n1 = q.front();
            q.pop();
            auto n2 = q.front();
            q.pop();
            auto n3 = q.front();
            q.pop();
            q.push(new Composite(n1, n2, n3));
        }
        root = q.front();
        auto *curSeq = new std::list<char>;
        map = new std::unordered_map<uint8_t, std::string>;
        root->fillMap(map, curSeq);
    }

    const std::string &HuffmanCode::encodeByte(uint8_t byte) {
        return (*map)[byte];
    }

    uint8_t HuffmanCode::decodeToByte(const std::string &seq, int *pos) {
        return root->decode(seq, pos);
    }
}
