add_library(XOR_based_DNA_encoding SHARED nucleotides.cpp nucleotides.h strands.cpp strands.h huffmanCode.cpp huffmanCode.h settings.cpp settings.h strandDecode.cpp strandDecode.h)
add_executable(XOR_based_DNA_encoding_main main.cpp nucleotides.cpp nucleotides.h strands.cpp strands.h huffmanCode.cpp huffmanCode.h settings.cpp settings.h strandDecode.cpp strandDecode.h)
