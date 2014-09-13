CC=g++
CFLAGS=-c -Wall -g -std=c++11  
LDFLAGS=
SOURCES = src/CGeometryTraversalUnit.cpp src/COctree.cpp  src/CRayGenerationUnit.cpp  src/CScene.cpp  src/CSimulator.cpp  src/Main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
TARGET  = theia_sim
EXECUTABLE = bin/$(TARGET)

all: $(SOURCES) $(TARGET)
	
$(TARGET): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(EXECUTABLE)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@


clean:
	$(RM) -f src/*.o $(EXECUTABLE)
