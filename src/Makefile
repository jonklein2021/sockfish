# Compiler & flags
CXX             = g++
LD              = g++
CXXFLAGS        = -I include -MMD -O3 -std=c++17 -Wall -Wextra -pedantic # -Werror
LDFLAGS         = -lsfml-graphics -lsfml-window -lsfml-system

# Directories
ODIR            := ./build
$(shell mkdir -p $(ODIR))

# Input files
CXXFILES        = game
TARGETS         = main

# Generate names of obj, exe, and d files
COMMONOFILES    = $(patsubst %, $(ODIR)/%.o, $(CXXFILES))
ALLOFILES       = $(patsubst %, $(ODIR)/%.o, $(CXXFILES) $(TARGETS))
EXEFILES        = $(patsubst %, $(ODIR)/%.exe, $(TARGETS))
DFILES          = $(patsubst %.o, %.d, $(ALLOFILES))

.DEFAULT_GOAL = all
.PRECIOUS: $(OFILES)
.PHONY: all clean

# Build all .exe files
all: $(EXEFILES)

# Purge the build folder
clean:
	@echo Cleaning up...
	@rm -rf $(ODIR)

# .cpp -> .o
$(ODIR)/%.o: %.cpp
	@echo "[CXX] $< --> $@"
	@$(CXX) $< -o $@ -c $(CXXFLAGS)

# .cpp -> .o
$(ODIR)/%.o: backend/%.cpp
	@echo "[CXX] $< --> $@"
	@$(CXX) $< -o $@ -c $(CXXFLAGS)

# .o -> .exe
$(ODIR)/%.exe: $(ODIR)/%.o $(COMMONOFILES)
	@echo "[LD] $^ --> $@"
	@$(LD) $^ -o $@ $(LDFLAGS)

-include $(DFILES)