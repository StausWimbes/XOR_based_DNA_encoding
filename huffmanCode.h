#ifndef XOR_BASED_DNA_ENCODING_HUFFMANCODE_H
#define XOR_BASED_DNA_ENCODING_HUFFMANCODE_H

#include <unordered_map>
#include <list>
#include <string>
#include <cstdint>

namespace xorCode {

    struct TreeNode {

        uint8_t byte;
        bool leaf;
        TreeNode *left, *middle, *right;

        TreeNode(TreeNode *left, TreeNode *middle, TreeNode *right);

        explicit TreeNode(uint8_t digit);

        void fillMap(std::unordered_map<uint8_t, std::string> *map, std::list<char> *curSeq) const;
    };

    class HuffmanCode {

    private:
        static HuffmanCode *instance;
        TreeNode *root;
        std::unordered_map<uint8_t, std::string> *map;

        HuffmanCode();

    public:

        HuffmanCode(HuffmanCode &other) = delete;

        void operator=(const HuffmanCode &other) = delete;

        static HuffmanCode *getInstance();

        const std::string &encodeByte(uint8_t byte);

        uint8_t decodeToByte(const std::string &seq, int *pos);

    };

}

#endif //XOR_BASED_DNA_ENCODING_HUFFMANCODE_H
