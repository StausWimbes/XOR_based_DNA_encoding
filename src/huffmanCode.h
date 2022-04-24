#ifndef XOR_CODE_HUFFMANCODE_H
#define XOR_CODE_HUFFMANCODE_H

#include <unordered_map>
#include <list>
#include <string>
#include <cstdint>

#include "settings.h"

namespace xorCode {

    struct TreeNode {

        virtual void fillMap(std::unordered_map<uint8_t, types::ternaries> &map, types::ternaries &seq) = 0;

        virtual void decode(types::iter &itBegin, types::iter &itEnd, types::bytes &out) const = 0;

    };

    struct Leaf : public TreeNode {

        uint8_t byte;

        explicit Leaf(uint8_t byte);

        void fillMap(std::unordered_map<uint8_t, types::ternaries> &map, types::ternaries &seq) override;

        void decode(types::iter &itBegin, types::iter &itEnd, types::bytes &out) const override;
    };

    struct ExtraLeaf : public TreeNode {

        types::ternaries extraSeq;

        void fillMap(std::unordered_map<uint8_t, types::ternaries> &map, types::ternaries &seq) override;

        void decode(types::iter &itBegin, types::iter &itEnd, types::bytes &out) const override;
    };

    struct Composite : public TreeNode {

        TreeNode *left, *middle, *right;

        Composite(TreeNode *left, TreeNode *middle, TreeNode *right);

        void fillMap(std::unordered_map<uint8_t, types::ternaries> &map, types::ternaries &seq) override;

        void decode(types::iter &itBegin, types::iter &itEnd, types::bytes &out) const override;
    };

    class HuffmanCode {

    private:
        static HuffmanCode *instance;
        TreeNode *root;
        std::unordered_map<uint8_t, types::ternaries> *map;
        ExtraLeaf *extraLeaf;

        HuffmanCode();

    public:

        HuffmanCode(HuffmanCode &other) = delete;

        void operator=(const HuffmanCode &other) = delete;

        static HuffmanCode *getInstance();

        const types::ternaries &encodeToTernaries(uint8_t byte);

        const types::ternaries &getExtraTernaries();

        void decodeToByte(types::iter &itBegin, types::iter &itEnd, types::bytes &out);

    };

}

#endif //XOR_CODE_HUFFMANCODE_H
