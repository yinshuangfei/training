
NAME=test

all: 01-compile

.PHONY: 01-compile
01-compile:
	rustc $@.rs -o $(NAME)_$@

clean:
	rm -fr $(NAME)_*