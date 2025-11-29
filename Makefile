# Compiler & flags
CXX        = g++
LD         = g++

# Detect Homebrew installation path
HOMEBREW_PATH := $(shell brew --prefix 2>/dev/null || echo /usr/local)

# Change this to where SFML 3.0 is installed
SFML_PATH = "/home/${USER}/sfml"

CXXFLAGS   = -I src/common -I src/backend -I src/engine -I$(HOMEBREW_PATH)/include -I$(SFML_PATH)/include -O3 -MMD -std=c++17 -Wall -Wextra -Wno-parentheses -pedantic
LDFLAGS    = -L$(HOMEBREW_PATH)/lib -L$(SFML_PATH)/lib -lsfml-graphics -lsfml-window -lsfml-system
export LD_LIBRARY_PATH=$(SFML_PATH)/lib 

# Directories
ODIR       = bin
SDIRS      = src/backend src/engine src/game/cli src/game/gui
$(shell mkdir -p $(ODIR))

# Gather all .cpp files
COMMONCXX 	 	= $(foreach dir, $(SDIRS), $(wildcard $(dir)/*.cpp))
ALLCXX	    	= $(wildcard *.cpp $(COMMONCXX))
COMMONOFILES	= $(patsubst src/%.cpp, $(ODIR)/%.o, $(COMMONCXX))
ALLOFILES    	= $(patsubst src/%.cpp, $(ODIR)/%.o, $(patsubst %.cpp, $(ODIR)/%.o, $(filter-out $(COMMONCXX), $(ALLCXX))))
DFILES			= $(ALLOFILES:.o=.d)

# Targets
TARGETS    = main test/test
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
$(ODIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	@echo "[CXX] $< --> $@"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

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
