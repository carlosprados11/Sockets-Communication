
//Cliente de eco sobre el protocolo TCP.

#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/unistd.h>
#include<netdb.h>

#define SERV_TCP_PORT 7500 // Puerto que utiliza el servidor para eco
#define SERV_HOST_ADDR "157.88.201.95"
#define MAXLINE 512

main(int argc, char *argv[])
{
    //Variables de programa

    int sockfd, sockConect, sockWrite, sockRead, cerrarSocket;
    struct sockaddr_in serv_addr;
    char DATA[MAXLINE];
    char Recibido[MAXLINE];

    //Ponemos a 0 la estructura de direcciones remotas
    bzero((char *) &serv_addr, sizeof(serv_addr));

    //Rellenamos la estructura de direcciones remotas
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR);
    serv_addr.sin_port = htons(SERV_TCP_PORT);

    /*Creamos un socket para que el cliente pueda enviar y recibir mensajes
    AF_INET para sockets IPv4, SOCK_STREAM para sockets TCP y 0 porque es una aplicacion típica*/
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    //Comprobamos errores
    if (sockfd<0)
    {
        perror("client: can't open stream socket");
        exit(EXIT_FAILURE);
    }

    //Conectamos el socket con el servidor
    sockConect=connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr));
    //Comprobamos errores
    if (sockConect < 0)
    {
        perror("client: can't connect to server");
        exit(EXIT_FAILURE);
    }

    //Leemos el mensaje
    printf("\nINTRODUCIR MENSAJE: ");
    gets(DATA);
    printf("\n");

    //Escribimos en el socket
    sockWrite=write(sockfd, DATA, sizeof(DATA));
    //Comprobamos errores
    if (sockWrite < 0)
    {
        perror("writing on stream socket");
        exit(EXIT_FAILURE);
    }

    //Leemos el eco del socket
    sockRead=read(sockfd, Recibido, sizeof(Recibido));
    //Comprobamos errores
    if (sockRead < 0)
    {
        perror("reading stream message");
        exit(EXIT_FAILURE);
    }

    //Imprimimos el eco recibido.
    printf("Datos recibidos: %s\n\n", Recibido);

    //Cerramos el socket
    cerrarSocket=close(sockfd);
    //Comprobamos errores
    if(cerrarSocket<0)
    {
        perror("close()");
        exit(EXIT_FAILURE);
    }

    exit(0);
}
