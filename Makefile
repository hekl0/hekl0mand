shell: shell.c CustomIO/CustomIO.c Command/Command.c JobController/JobController.c
	$(CC) -g -o shell $^

clean:
	rm -f shell mp1.tar.gz

submission: clean
	(rm -f csc256-mp1 ; tar -cvf - . | gzip > mp1.tar.gz)
