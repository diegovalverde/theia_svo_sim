CC=g++
CFLAGS=-c -g -std=c++11  
LDFLAGS=-lreadline -lncurses
SOURCES = src/CGeometryTraversalUnit.cpp src/CMemory.cpp src/Common.cpp src/CGpu.cpp src/COctree.cpp  src/CRayGenerationUnit.cpp  src/CScene.cpp  src/CSimulator.cpp  src/Main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
TARGET  = theia_sim
EXECUTABLE = bin/$(TARGET)

all: $(SOURCES) $(TARGET)
	
$(TARGET): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS)  $< -o $@


clean:
	$(RM) -f src/*.o $(EXECUTABLE)
