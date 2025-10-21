# Compiler & flags
CXX        = g++
LD         = g++

# Detect Homebrew installation path
HOMEBREW_PREFIX := $(shell brew --prefix 2>/dev/null || echo /usr/local)

CXXFLAGS   = -I common -I backend -I engine -I$(HOMEBREW_PREFIX)/include -MMD -O3 -std=c++17 -Wall -Wextra -Wno-parentheses -pedantic
LDFLAGS    = -L$(HOMEBREW_PREFIX)/lib -lsfml-graphics -lsfml-window -lsfml-system

# Directories
ODIR       = build
SDIRS      = backend engine game/cli game/gui
$(shell mkdir -p $(ODIR))

# Gather all .cpp files
COMMONCXX 	 	= $(foreach dir, $(SDIRS), $(wildcard $(dir)/*.cpp))
ALLCXX	    	= $(wildcard *.cpp $(COMMONCXX))
COMMONOFILES	= $(patsubst %.cpp, $(ODIR)/%.o, $(COMMONCXX))
ALLOFILES    	= $(patsubst %.cpp, $(ODIR)/%.o, $(ALLCXX))
DFILES			= $(ALLOFILES:.o=.d)

# Targets
TARGETS    = main test
EXEFILES   = $(patsubst %, $(ODIR)/%.exe, $(TARGETS))

.DEFAULT_GOAL = all
.PRECIOUS: $(COMMONOFILES)
.PHONY: all clean

# Build all executables
all: $(EXEFILES)

# Clean build directory
clean:
	@echo Cleaning up...
	@rm -rf $(ODIR)

# Compile all .cpp -> .o files (merged into one rule)
$(ODIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "[CXX] $< --> $@"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Link .o files into .exe
$(ODIR)/%.exe: $(ODIR)/%.o $(COMMONOFILES)
	@echo "[LD] $^ --> $@"
	@$(LD) $^ -o $@ $(LDFLAGS)

# Include d files for faster incremental builds
-include $(DFILES)