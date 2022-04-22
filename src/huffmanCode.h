#ifndef XOR_CODE_HUFFMANCODE_H
#define XOR_CODE_HUFFMANCODE_H

#include <unordered_map>
#include <list>
#include <string>
#include <cstdint>

#include "settings.h"

namespace xorCode {

    struct TreeNode {

        virtual void fillMap(std::unordered_map<uint8_t, std::string> *map, types::ternaries *seq) const = 0;

        virtual void decode(types::iter &itBegin, types::iter &itEnd, types::bytes &out) const = 0;

    };

    struct Leaf : public TreeNode {

        uint8_t byte;

        explicit Leaf(uint8_t byte);

        void fillMap(std::unordered_map<uint8_t, std::string> *map, types::ternaries *seq) const override;

        void decode(types::iter &itBegin, types::iter &itEnd, types::bytes &out) const override;
    };

    struct ExtraLeaf : public TreeNode {

        void fillMap(std::unordered_map<uint8_t, std::string> *map, types::ternaries *seq) const override;

        void decode(types::iter &itBegin, types::iter &itEnd, types::bytes &out) const override;
    };

    struct Composite : public TreeNode {

        TreeNode *left, *middle, *right;

        Composite(TreeNode *left, TreeNode *middle, TreeNode *right);

        void fillMap(std::unordered_map<uint8_t, std::string> *map, types::ternaries *seq) const override;

        void decode(types::iter &itBegin, types::iter &itEnd, types::bytes &out) const override;
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

        void decodeToByte(types::iter &itBegin, types::iter &itEnd, types::bytes &out);

    };

}

#endif //XOR_CODE_HUFFMANCODE_H
