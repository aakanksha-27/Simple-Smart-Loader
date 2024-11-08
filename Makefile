#invoke make inside following directories and in this order: loader, launch, fib
all:
	$(MAKE) -C loader
	$(MAKE) -C launcher
	$(MAKE) -C test

#move the lib_simpleloader.so and launch binaries inside bin directory

#Provide the command for cleanup
