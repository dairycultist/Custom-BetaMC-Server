.PHONY: run

a.out: *.c *.h
	@gcc *.c

# server_app next

run: a.out
	@./a.out

clean:
	@rm a.out