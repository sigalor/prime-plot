# build structure paths
PROJECT_BIN_DIR=bin
PROJECT_INCLUDE_DIR=include
PROJECT_OBJ_DIR=obj
PROJECT_SRC_DIR=src

CPP_FILES=$(wildcard $(PROJECT_SRC_DIR)/*.cpp)
OBJ_FILES=$(addprefix $(PROJECT_OBJ_DIR)/,$(notdir $(CPP_FILES:.cpp=.o)))
EXECUTABLE=$(PROJECT_BIN_DIR)/prime-plot

CXX=g++
CXXFLAGS=-O3 -std=c++14 -Wall -I$(PROJECT_INCLUDE_DIR)
LDFLAGS=-O3

all: $(CPP_FILES) $(EXECUTABLE)

clean: cleanexe
	rm -f $(OBJ_FILES) 2>&1 > /dev/null

cleanexe:
	rm -f $(EXECUTABLE) 2>&1 > /dev/null

$(EXECUTABLE): $(OBJ_FILES)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(PROJECT_OBJ_DIR)/%.o: $(PROJECT_SRC_DIR)/%.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

.PHONY: all clean cleanexe
