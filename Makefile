interfaces=Aeolus.js Aeolus1.js Aeolus2.js
all: $(interfaces)

# You need a symlink to the stops directory named "stops" in
# the current directory

%.js: stops2control.py
	./stops2control.py stops/$* > $@

clean: 
	rm -f $(interfaces)

#.PHONY: $(interfaces)
