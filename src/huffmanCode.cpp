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

    void Leaf::decode(iter &itBegin, iter &itEnd, std::list<uint8_t> &output) const {
        output.push_back(byte);
    }

    void ExtraLeaf::fillMap(std::unordered_map<uint8_t, std::string> *map, std::list<char> *curSeq) const {
        // do nothing because these leafs are duplicates to fill up the tree
    }

    void ExtraLeaf::decode(iter &itBegin, iter &itEnd, std::list<uint8_t> &output) const {
        itBegin = itEnd;
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

    void Composite::decode(iter &itBegin, iter &itEnd, std::list<uint8_t> &output) const {
        if (itBegin != itEnd) {
            switch (*itBegin) {
                case '0':
                    ++itBegin;
                    return left->decode(itBegin, itEnd, output);
                case '1':
                    ++itBegin;
                    return middle->decode(itBegin, itEnd, output);
                case '2':
                    ++itBegin;
                    return right->decode(itBegin, itEnd, output);
                default:
                    ++itBegin;
                    return left->decode(itBegin, itEnd, output);
            }
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
        q.push(new ExtraLeaf());
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

    void HuffmanCode::decodeToByte(iter &itBegin, iter &itEnd, std::list<uint8_t> &output) {
        root->decode(itBegin, itEnd, output);
    }
}
