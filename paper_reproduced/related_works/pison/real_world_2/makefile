DIR = bin
EXEC5 = $(DIR)/real_world
TARGET = ${EXEC5} 
all: $(TARGET)

CC = g++
CC_FLAGS = -O3 -std=c++11 -mavx -mavx2 -msse -msse2 -msse4 -msse4.2 -mpclmul
POST_FLAGS = -lpthread -mcmodel=medium -static-libstdc++

SOURCE5 = src/*.cpp example/real_world.cpp
$(EXEC5): $(SOURCE5)
	mkdir -p $(DIR)
	$(CC) $(CC_FLAGS) -o $(EXEC5) $(SOURCE5) $(POST_FLAGS)

clean:
	-$(RM) $(TARGET)
