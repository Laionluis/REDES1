# regras de ligação
teste: teste.o fila.o
	gcc teste.o fila.o -o teste -lpthread

# regras de compilação
teste.o: teste.c fila.h
	gcc -c teste.c -lpthread
 
fila.o: fila.c fila.h
	gcc -c -fpic fila.c 
	gcc -shared -o libfila.so fila.o

clean:
	-rm *~ *.o