all: compile link
	@echo "Done! Run with ./build/bin/main"

compile:
	@echo "Compiling..."
	@mkdir -p build/obj
	@g++ -c main.cpp
	@mv main.o build/obj

link:
	@echo "Linking..."
	@mkdir -p build/bin
	@g++ build/obj/main.o -o build/bin/main -lsfml-graphics -lsfml-window -lsfml-system

clean:
	@echo "Cleaning..."
	@rm -rf build
