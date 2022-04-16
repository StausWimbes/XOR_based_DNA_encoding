set(HEADER_FILES
        nucleotides.h
        strands.h
        huffmanCode.h
        settings.h
        strandDecode.h
        )

set(SOURCE_FILES
        nucleotides.cpp
        strands.cpp
        huffmanCode.cpp
        settings.cpp
        strandDecode.cpp
        )

add_library(xorCode SHARED ${SOURCE_FILES} ${HEADER_FILES})
