BINARY = BoidSimulate

all:
	cmake -B build
	glslc boids.comp -o boids.comp.spv
	glslc boids.vert -o boids.vert.spv
	glslc boids.frag -o boids.frag.spv
	make -C build/
	
	rm -rf $(BINARY)
	mv build/$(BINARY) .

clean:
	rm -rf $(BINARY)

fclean: clean
	rm -rf build

re: fclean all