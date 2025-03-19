CC = gcc
CXX = g++
CFLAGS = -O2 -w -g
CXXFLAGS = -O2 -w -g

# Output executables
DLL_OBJ = dll.o
DEBUG_OBJ = debug.o
DP_OBJ = dp.o

# Targets
all: dll debug dp

dll: $(DLL_OBJ)
	$(CC) -o dll $(DLL_OBJ)

debug: $(DEBUG_OBJ)
	$(CXX) -o debug $(DEBUG_OBJ)

dp: $(DP_OBJ)
	$(CXX) -o dp $(DP_OBJ)

# Compilation rules
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f *.o dll debug dp
