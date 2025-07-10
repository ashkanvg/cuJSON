DIR = bin
EXEC2 = $(DIR)/bestbuy
EXEC3 = $(DIR)/google
EXEC4 = $(DIR)/nspl
EXEC5 = $(DIR)/twitter
EXEC9 = $(DIR)/walmart
EXEC10 = $(DIR)/wiki
TARGET = ${EXEC2} ${EXEC3} ${EXEC4} ${EXEC5} ${EXEC9} ${EXEC10} 
all: $(TARGET)

CC = g++
CC_FLAGS = -O3 -std=c++11 -mavx -mavx2 -msse -msse2 -msse4 -msse4.2 -mpclmul
POST_FLAGS = -lpthread -mcmodel=medium -static-libstdc++ -fopenmp

SOURCE2 = src/*.cpp example/bestbuy.cpp
$(EXEC2): $(SOURCE2)
	mkdir -p $(DIR)
	$(CC) $(CC_FLAGS) -o $(EXEC2) $(SOURCE2) $(POST_FLAGS)

SOURCE3 = src/*.cpp example/google.cpp
$(EXEC3): $(SOURCE3)
	mkdir -p $(DIR)
	$(CC) $(CC_FLAGS) -o $(EXEC3) $(SOURCE3) $(POST_FLAGS)

SOURCE4 = src/*.cpp example/nspl.cpp
$(EXEC4): $(SOURCE4)
	mkdir -p $(DIR)
	$(CC) $(CC_FLAGS) -o $(EXEC4) $(SOURCE4) $(POST_FLAGS)

SOURCE5 = src/*.cpp example/twitter.cpp
$(EXEC5): $(SOURCE5)
	mkdir -p $(DIR)
	$(CC) $(CC_FLAGS) -o $(EXEC5) $(SOURCE5) $(POST_FLAGS)

SOURCE9 = src/*.cpp example/walmart.cpp
$(EXEC9): $(SOURCE9)
	mkdir -p $(DIR)
	$(CC) $(CC_FLAGS) -o $(EXEC9) $(SOURCE9) $(POST_FLAGS)

SOURCE10 = src/*.cpp example/wiki.cpp
$(EXEC10): $(SOURCE10)
	mkdir -p $(DIR)
	$(CC) $(CC_FLAGS) -o $(EXEC10) $(SOURCE10) $(POST_FLAGS)


clean:
	-$(RM) $(TARGET)
