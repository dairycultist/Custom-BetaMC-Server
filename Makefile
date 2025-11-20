.PHONY: run clean

a.out: *.c *.h
	@gcc *.c

run: a.out
	@./a.out

clean:
	@rm a.out