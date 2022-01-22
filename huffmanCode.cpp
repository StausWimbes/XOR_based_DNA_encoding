#include <cstdint>
#include <queue>
#include <unordered_map>
#include <list>
#include "huffmanCode.h"

namespace xorCode {

    TreeNode::TreeNode(TreeNode *left, TreeNode *middle, TreeNode *right) {
        this->byte = 0;
        leaf = false;
        this->left = left;
        this->middle = middle;
        this->right = right;
    }

    TreeNode::TreeNode(uint8_t digit) {
        this->byte = digit;
        leaf = true;
        left = middle = right = nullptr;
    }

    void TreeNode::fillMap(std::unordered_map<uint8_t, std::string> *map, std::list<char> *curSeq) const {
        if (leaf) {
            std::string value;
            for (auto c: *curSeq) {
                value.append(sizeof(char), c);
            }
            (*map)[byte] = value;
        } else {
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
            q.push(new TreeNode(byte));
            ++byte;
        }
        // add an extra node so that every node later on will have exactly three children
        q.push(new TreeNode(255));
        while (q.size() > 1) {
            auto n1 = q.front();
            q.pop();
            auto n2 = q.front();
            q.pop();
            auto n3 = q.front();
            q.pop();
            q.push(new TreeNode(n1, n2, n3));
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
        auto curNode = root;
        while (!curNode->leaf) {
            switch (seq.front()) {
                case '0':
                    curNode = curNode->left;
                    break;
                case '1':
                    curNode = curNode->middle;
                    break;
                case '2':
                    curNode = curNode->right;
                    break;
                default:
                    curNode = curNode->left;
            }
            ++(*pos);
        }
        return curNode->byte;
    }
}
