set(HEADER_FILES
        nucleotideUtils.h
        encode.h
        huffmanCode.h
        settings.h
        decode.h
        )

set(SOURCE_FILES
        nucleotideUtils.cpp
        encode.cpp
        huffmanCode.cpp
        settings.cpp
        decode.cpp
        )

add_library(xorCode STATIC ${SOURCE_FILES} ${HEADER_FILES})
