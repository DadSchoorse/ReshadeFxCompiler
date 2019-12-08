DIRS = src

compile:
	for i in $(DIRS); do $(MAKE) -C $$i; done


