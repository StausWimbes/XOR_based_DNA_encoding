#ifndef XOR_CODE_HUFFMANCODE_H
#define XOR_CODE_HUFFMANCODE_H

#include <unordered_map>
#include <list>
#include <string>
#include <cstdint>

namespace xorCode {

    typedef std::list<char>::iterator iter;

    struct TreeNode {

        virtual void fillMap(std::unordered_map<uint8_t, std::string> *map, std::list<char> *curSeq) const = 0;

        virtual void decode(iter &itBegin, iter &itEnd, std::list<uint8_t> &output) const = 0;

    };

    struct Leaf : public TreeNode {

        uint8_t byte;

        explicit Leaf(uint8_t byte);

        void fillMap(std::unordered_map<uint8_t, std::string> *map, std::list<char> *curSeq) const override;

        void decode(iter &itBegin, iter &itEnd, std::list<uint8_t> &output) const override;
    };

    struct ExtraLeaf : public TreeNode {

        void fillMap(std::unordered_map<uint8_t, std::string> *map, std::list<char> *curSeq) const override;

        void decode(iter &itBegin, iter &itEnd, std::list<uint8_t> &output) const override;
    };

    struct Composite : public TreeNode {

        TreeNode *left, *middle, *right;

        Composite(TreeNode *left, TreeNode *middle, TreeNode *right);

        void fillMap(std::unordered_map<uint8_t, std::string> *map, std::list<char> *curSeq) const override;

        void decode(iter &itBegin, iter &itEnd, std::list<uint8_t> &output) const override;
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

        void decodeToByte(iter &itBegin, iter &itEnd, std::list<uint8_t> &output);

    };

}

#endif //XOR_CODE_HUFFMANCODE_H
