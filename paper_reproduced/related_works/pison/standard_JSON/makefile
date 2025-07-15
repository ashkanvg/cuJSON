DIR = bin
EXEC5 = $(DIR)/bestbuy
EXEC6 = $(DIR)/google
EXEC7 = $(DIR)/nspl
EXEC8 = $(DIR)/twitter
EXEC12 = $(DIR)/walmart
EXEC13 = $(DIR)/wiki
TARGET = ${EXEC5} ${EXEC6} ${EXEC7} ${EXEC8} ${EXEC12} ${EXEC13}
all: $(TARGET)

CC = g++
CC_FLAGS = -O3 -std=c++11 -mavx -mavx2 -msse -msse2 -msse4 -msse4.2 -mpclmul
POST_FLAGS = -lpthread -mcmodel=medium -static-libstdc++

SOURCE8 = src/*.cpp example/twitter.cpp
$(EXEC8): $(SOURCE8)
	mkdir -p $(DIR)
	$(CC) $(CC_FLAGS) -o $(EXEC8) $(SOURCE8) $(POST_FLAGS)

SOURCE5 = src/*.cpp example/bestbuy.cpp
$(EXEC5): $(SOURCE5)
	mkdir -p $(DIR)
	$(CC) $(CC_FLAGS) -o $(EXEC5) $(SOURCE5) $(POST_FLAGS)

SOURCE6 = src/*.cpp example/google.cpp
$(EXEC6): $(SOURCE6)
	mkdir -p $(DIR)
	$(CC) $(CC_FLAGS) -o $(EXEC6) $(SOURCE6) $(POST_FLAGS)

SOURCE7 = src/*.cpp example/nspl.cpp
$(EXEC7): $(SOURCE7)
	mkdir -p $(DIR)
	$(CC) $(CC_FLAGS) -o $(EXEC7) $(SOURCE7) $(POST_FLAGS)

SOURCE12 = src/*.cpp example/walmart.cpp
$(EXEC12): $(SOURCE12)
	mkdir -p $(DIR)
	$(CC) $(CC_FLAGS) -o $(EXEC12) $(SOURCE12) $(POST_FLAGS)

SOURCE13 = src/*.cpp example/wiki.cpp
$(EXEC13): $(SOURCE13)
	mkdir -p $(DIR)
	$(CC) $(CC_FLAGS) -o $(EXEC13) $(SOURCE13) $(POST_FLAGS)


clean:
	-$(RM) $(TARGET)
