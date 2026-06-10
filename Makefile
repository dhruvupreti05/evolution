SRCDIR = ./src
INCDIR = $(SRCDIR)/include
OBJDIR = ./build
LIBRARY = /opt/homebrew/Cellar/sfml/2.6.1

CC = g++
CXXFLAGS = -std=c++17 -I $(LIBRARY)/include -I $(INCDIR)
LDFLAGS = -L $(LIBRARY)/lib -lsfml-graphics -lsfml-window -lsfml-system

APPNAME = evolution

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

$(shell mkdir -p $(OBJDIR))

$(APPNAME): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(APPNAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) -c $< $(CXXFLAGS) -o $@

clean:
	rm -f $(OBJDIR)/*.o $(APPNAME)
