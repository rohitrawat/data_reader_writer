OBJS=BinaryFile.o  FileStats.o  main.o  numeric.o  TextFile.o

test: ${OBJS}
	g++ -o test ${OBJS}
	echo Done. Run ./test

clean:
	rm test *.o
