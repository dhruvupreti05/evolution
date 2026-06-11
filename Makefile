SRCDIR = ./src
OBJDIR = ./build
LIBRARY = /opt/homebrew/Cellar/sfml/2.6.1

CC = g++
CXXFLAGS = -std=c++17 -I $(LIBRARY)/include -I $(SRCDIR)
LDFLAGS = -L $(LIBRARY)/lib -lsfml-graphics -lsfml-window -lsfml-system

APPNAME = evolution

SOURCES = $(shell find $(SRCDIR) -name "*.cpp")
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

$(shell mkdir -p $(OBJDIR))

$(APPNAME): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(APPNAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) -c $< $(CXXFLAGS) -o $@

clean:
	rm -rf $(OBJDIR) $(APPNAME)