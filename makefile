# Nome do executável
TARGET = trabalhocg

# Diretórios
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = headers

# Compilador e flags
CXX = g++
CXXFLAGS = -g -I$(INCLUDE_DIR)
LINKING = -lglut -lGL -lGLU

# Lista de arquivos fonte e objetos
SOURCES = main.cpp $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir $(SOURCES)))

# Regras
all: build $(TARGET)

build:
	mkdir -p $(BUILD_DIR)

# Compilar os arquivos .cpp em objetos .o
$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ 

# Link dos objetos para criar o executável
$(TARGET): $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) -lm $(LINKING)

run: all $(TARGET)
	./$(TARGET) arena_teste.svg

clean:
	rm -rf $(TARGET) $(BUILD_DIR)
