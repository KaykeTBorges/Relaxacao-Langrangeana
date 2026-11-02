CXX = g++
CXXFLAGS = -O3 -Wall -std=c++17

TARGET = relaxacao

SRCS = main.cpp data.cpp node.cpp kruskal.cpp subgradiente.cpp bnb.cpp
HEADERS = data.h node.h kruskal.h subgradiente.h bnb.h

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