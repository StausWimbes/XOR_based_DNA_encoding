#ifndef XOR_CODE_HUFFMANCODE_H
#define XOR_CODE_HUFFMANCODE_H

#include <unordered_map>
#include <list>
#include <string>
#include <cstdint>

namespace xorCode {

    struct TreeNode {

        virtual void fillMap(std::unordered_map<uint8_t, std::string> *map, std::list<char> *curSeq) const = 0;

        virtual uint8_t decode(const std::string &seq, int &pos) const = 0;

    };

    struct Leaf : public TreeNode {

        uint8_t byte;

        explicit Leaf(uint8_t byte);

        void fillMap(std::unordered_map<uint8_t, std::string> *map, std::list<char> *curSeq) const override;

        uint8_t decode(const std::string &seq, int &pos) const override;
    };

    struct ExtraLeaf : public TreeNode {

        void fillMap(std::unordered_map<uint8_t, std::string> *map, std::list<char> *curSeq) const override;

        uint8_t decode(const std::string &seq, int &pos) const override;
    };

    struct Composite : public TreeNode {

        TreeNode *left, *middle, *right;

        Composite(TreeNode *left, TreeNode *middle, TreeNode *right);

        void fillMap(std::unordered_map<uint8_t, std::string> *map, std::list<char> *curSeq) const override;

        uint8_t decode(const std::string &seq, int &pos) const override;
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

        uint8_t decodeToByte(const std::string &seq, int &pos);

    };

}

#endif //XOR_CODE_HUFFMANCODE_H
