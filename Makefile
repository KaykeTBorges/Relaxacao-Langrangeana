CXX = g++
CXXFLAGS = -O3 -Wall -std=c++17 -Iinclude

TARGET = relaxacao

SRCS = src/main.cpp src/data.cpp src/node.cpp src/kruskal.cpp src/subgradiente.cpp src/bnb.cpp
HEADERS = include/data.h include/node.h include/kruskal.h include/subgradiente.h include/bnb.h

# Instância padrão caso nenhuma seja especificada no comando
INSTANCE ?= instancias/burma14.tsp

all: $(TARGET)

# Compilação direta de todos os .cpp para o binário sem gerar arquivos .o
$(TARGET): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

# Execução aceitando o parâmetro da instância
run: $(TARGET)
	./$(TARGET) $(INSTANCE)

clean:
	rm -f $(TARGET) a.out

.PHONY: all run clean