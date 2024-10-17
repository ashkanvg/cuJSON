DIR = bin
EXEC1 = $(DIR)/example1
EXEC2 = $(DIR)/example2
EXEC3 = $(DIR)/example3
EXEC4 = $(DIR)/example4
EXEC5 = $(DIR)/example5
EXEC6 = $(DIR)/example6
EXEC7 = $(DIR)/example7
EXEC8 = $(DIR)/example8
EXEC9 = $(DIR)/example9
EXEC10 = $(DIR)/example10
EXEC11 = $(DIR)/example11
EXEC12 = $(DIR)/example12
EXEC13 = $(DIR)/example13
TARGET = $(EXEC1) ${EXEC2} ${EXEC3} ${EXEC4} ${EXEC5} ${EXEC6} ${EXEC7} ${EXEC8} ${EXEC9} ${EXEC10} ${EXEC11} ${EXEC12} ${EXEC13}
all: $(TARGET)

CC = g++
CC_FLAGS = -O3 -std=c++11 -mavx -mavx2 -msse -msse2 -msse4 -msse4.2 -mpclmul
POST_FLAGS = -lpthread -mcmodel=medium -static-libstdc++

SOURCE1 = src/*.cpp example/example1.cpp
$(EXEC1): $(SOURCE1)
	mkdir -p $(DIR)
	$(CC) $(CC_FLAGS) -o $(EXEC1) $(SOURCE1) $(POST_FLAGS)

SOURCE2 = src/*.cpp example/example2.cpp
$(EXEC2): $(SOURCE2)
	mkdir -p $(DIR)
	$(CC) $(CC_FLAGS) -o $(EXEC2) $(SOURCE2) $(POST_FLAGS)

SOURCE3 = src/*.cpp example/example3.cpp
$(EXEC3): $(SOURCE3)
	mkdir -p $(DIR)
	$(CC) $(CC_FLAGS) -o $(EXEC3) $(SOURCE3) $(POST_FLAGS)

SOURCE4 = src/*.cpp example/example4.cpp
$(EXEC4): $(SOURCE4)
	mkdir -p $(DIR)
	$(CC) $(CC_FLAGS) -o $(EXEC4) $(SOURCE4) $(POST_FLAGS)

SOURCE5 = src/*.cpp example/example5.cpp
$(EXEC5): $(SOURCE5)
	mkdir -p $(DIR)
	$(CC) $(CC_FLAGS) -o $(EXEC5) $(SOURCE5) $(POST_FLAGS)

SOURCE6 = src/*.cpp example/example6.cpp
$(EXEC6): $(SOURCE6)
	mkdir -p $(DIR)
	$(CC) $(CC_FLAGS) -o $(EXEC6) $(SOURCE6) $(POST_FLAGS)

SOURCE7 = src/*.cpp example/example7.cpp
$(EXEC7): $(SOURCE7)
	mkdir -p $(DIR)
	$(CC) $(CC_FLAGS) -o $(EXEC7) $(SOURCE7) $(POST_FLAGS)

SOURCE8 = src/*.cpp example/example8.cpp
$(EXEC8): $(SOURCE8)
	mkdir -p $(DIR)
	$(CC) $(CC_FLAGS) -o $(EXEC8) $(SOURCE8) $(POST_FLAGS)

SOURCE9 = src/*.cpp example/example9.cpp
$(EXEC9): $(SOURCE9)
	mkdir -p $(DIR)
	$(CC) $(CC_FLAGS) -o $(EXEC9) $(SOURCE9) $(POST_FLAGS)

SOURCE10 = src/*.cpp example/example10.cpp
$(EXEC10): $(SOURCE10)
	mkdir -p $(DIR)
	$(CC) $(CC_FLAGS) -o $(EXEC10) $(SOURCE10) $(POST_FLAGS)

SOURCE11 = src/*.cpp example/example11.cpp
$(EXEC11): $(SOURCE11)
	mkdir -p $(DIR)
	$(CC) $(CC_FLAGS) -o $(EXEC11) $(SOURCE11) $(POST_FLAGS)

SOURCE12 = src/*.cpp example/example12.cpp
$(EXEC12): $(SOURCE12)
	mkdir -p $(DIR)
	$(CC) $(CC_FLAGS) -o $(EXEC12) $(SOURCE12) $(POST_FLAGS)

SOURCE13 = src/*.cpp example/example13.cpp
$(EXEC13): $(SOURCE13)
	mkdir -p $(DIR)
	$(CC) $(CC_FLAGS) -o $(EXEC13) $(SOURCE13) $(POST_FLAGS)


clean:
	-$(RM) $(TARGET)
