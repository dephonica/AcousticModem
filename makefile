APP_NAME := ADSSS

SRCS_PATH := src/
SRCS :=

SRCS += $(SRCS_PATH)PcmNormalization.cpp
SRCS += $(SRCS_PATH)ReferenceSource.cpp
SRCS += $(SRCS_PATH)RandomGenerator.cpp
SRCS += $(SRCS_PATH)BitBuffer.cpp
SRCS += $(SRCS_PATH)FftEngineFftw.cpp
SRCS += $(SRCS_PATH)WindowFunctions.cpp
SRCS += $(SRCS_PATH)FftEngineHelper.cpp
SRCS += $(SRCS_PATH)EnvelopeGenerator.cpp
SRCS += $(SRCS_PATH)KernelConverter.cpp
SRCS += $(SRCS_PATH)FirKernelSource.cpp
SRCS += $(SRCS_PATH)FirBlockConvolver.cpp
SRCS += $(SRCS_PATH)Encoder.cpp
SRCS += $(SRCS_PATH)Decoder.cpp
SRCS += $(SRCS_PATH)Correlator2.cpp
SRCS += $(SRCS_PATH)CorrelatorsSet.cpp
SRCS += $(SRCS_PATH)StreamFrontend.cpp
SRCS += $(SRCS_PATH)StreamDecoder.cpp
SRCS += $(SRCS_PATH)StreamDetector.cpp
SRCS += $(SRCS_PATH)Payload.cpp
SRCS += $(SRCS_PATH)Configuration.cpp

GCC_FLAGS := -Wall -Wno-sign-compare -std=c++14 -fPIC -I include/ffmpeg

all:
	g++ $(GCC_FLAGS) $(SRCS) -shared -o lib$(APP_NAME).so -Llib/aarch64 -lswresample -lavutil -lfftw3f
