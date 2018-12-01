
#Makefile para la compilacion de los prog cliente_eco
#Limpia tambien los semaforos

todo: cliente robot cliente_b gestor

cliente: cliente_eco.o
	gcc -o cliente cliente_eco.o 

robot: cliente_srobot.o
	gcc -o robot cliente_srobot.o

cliente_b: cliente_b.o
	gcc -o cliente_b cliente_b.o 

gestor: gestor.o
	gcc -o gestor gestor.o 

cliente_eco.o: cliente_eco.c
	gcc -c cliente_eco.c
	
cliente_srobot.o: cliente_srobot.c
	gcc -c cliente_srobot.c


cliente_b.o: cliente_b.c
	gcc -c cliente_b.c

gestor.o: gestor.c
	gcc -c gestor.c

clean:
	rm cliente \
		cliente_eco.o
		
	rm robot \
		cliente_srobot.o

	rm cliente_b \
		cliente_b.o

	rm gestor \
		gestor.o