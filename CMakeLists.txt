cmake_minimum_required(VERSION 3.10)

project(ADSSS)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_DECODER "Build stream decoder" OFF)
option(ENABLE_ASAN "Build with address sanitizer" OFF)
option(BUILD_PROGRAMS "Build example programs" OFF)

if (ENABLE_DECODER)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(LIBSWRESAMPLE REQUIRED libswresample)
    pkg_search_module(LIBAVUTIL REQUIRED libavutil)
endif()

if (ENABLE_ASAN)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -W -Wall -Wextra -Wstack-protector -fstack-protector-all")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -W -Wall -Wextra -Wstack-protector -fstack-protector-all")

set(ADSSS_SRCS
    src/PcmNormalization.cpp
    src/ReferenceSource.cpp
    src/RandomGenerator.cpp
    src/BitBuffer.cpp
    src/kissfft/kiss_fft.c
    src/kissfft/kiss_fftr.c
    src/FftEngine.cpp
    src/WindowFunctions.cpp
    src/FftEngineHelper.cpp
    src/EnvelopeGenerator.cpp
    src/KernelConverter.cpp
    src/FirKernelSource.cpp
    src/FirBlockConvolver.cpp
    src/Encoder.cpp
    src/Payload.cpp
    src/Configuration.cpp
)

if (ENABLE_DECODER)
    list(APPEND ADSSS_SRCS
        src/Decoder.cpp
        src/Correlator2.cpp
        src/CorrelatorsSet.cpp
        src/StreamFrontend.cpp
        src/StreamDecoder.cpp
        src/StreamDetector.cpp
    )
endif()

set(ADSSS_LIBRARY ${PROJECT_NAME})
add_library(${ADSSS_LIBRARY} ${ADSSS_SRCS})
target_link_libraries(${ADSSS_LIBRARY} PUBLIC ${LIBSWRESAMPLE_LDFLAGS} ${LIBAVUTIL_LDFLAGS})
target_include_directories(${ADSSS_LIBRARY} PUBLIC ${LIBSWRESAMPLE_INCLUDE_DIRS} ${LIBAVUTIL_INCLUDE_DIRS})

if (BUILD_PROGRAMS)
    add_executable(${PROJECT_NAME}_encode src/EncodeExample.cpp)
    target_link_libraries(${PROJECT_NAME}_encode ${ADSSS_LIBRARY})

    if (ENABLE_DECODER)
        add_executable(${PROJECT_NAME}_decode src/DecodeExample.cpp)
        target_link_libraries(${PROJECT_NAME}_decode ${ADSSS_LIBRARY})
    endif()
endif()
