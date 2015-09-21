BIT=$(shell getconf LONG_BIT)
ifeq ($(BIT), 64)
    LIB=lib64
else
    LIB=lib32
endif

INCLUDES=	-I./\
			-I../../../include\
            -I../../../$(LIB)/ullib/include/\
			-I../../../$(LIB)/utillib/include\

LIBDIR=     -L./\
            -L../../../$(LIB)/ullib/lib -lullib\
			-L../../../$(LIB)/utillib/lib -lutil\


CXX     = g++ 
#CXXFLAGS    = -std=c++11 -g  -fPIC -Wall  -finline-functions   -Wno-unused-parameter   -Wno-unused-function $(INCLUDES)
CXXFLAGS    = -g  -fPIC -Wall  -finline-functions   -Wno-unused-parameter   -Wno-unused-function $(INCLUDES)
LDFLAGS     =  $(LIBDIR)  -lcurl -lpthread

TARGET = gettile_from_result.so 
SRC_DIR = ./

OBJS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
all: ${TARGET} output

%.o: %.cpp %.h 
	$(CXX) $(CXXFLAGS) $< -c -o $@ 

$(TARGET): ${OBJS}
	$(CXX) $(CXXFLAGS) -shared -o  $@  $^ $(LDFLAGS)

OUTPUTDIR = ../../../output/plugin

output:
	mkdir -p $(OUTPUTDIR)
	mv $(TARGET) $(OUTPUTDIR)

clean:
	rm -f ./*.o $(TARGET)
