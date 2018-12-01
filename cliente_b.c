
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
#include<sys/ipc.h>
#include<sys/msg.h>

#define SERV_TCP_PORT 7500 // Puerto que utiliza el servidor para eco
#define SERV_HOST_ADDR "157.88.201.95"
#define MAXLINE 512
#define MAXTEXT 100

struct MiMensaje
{
 long Id_Mensaje;
 int Dato_Numerico;
 char Mensaje[MAXTEXT];
}Un_Mensaje;

main(int argc, char *argv[])
{
    //Variables de programa
    int sockfd, sockConect, sockWrite, sockRead, cerrarSocket;
    struct sockaddr_in serv_addr;

    //Ponemos a 0 la estructura de direcciones remotas
    bzero((char *) &serv_addr, sizeof(serv_addr));

    //Rellenamos la estructura de direcciones remotas
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR);
    serv_addr.sin_port = htons(SERV_TCP_PORT);

    // Cola de mensajes
    key_t Clave1;
    int Id_Cola_Mensajes;

    // Obtengo una clave para la cola de mensajes
    Clave1 = ftok("/bin/ls", 35);
    // Compruebo errores
    if (Clave1 == (key_t)-1)
    {
        printf("Error al obtener clave para cola mensajes");
        return(0);
    }

    // Creo la cola de mensajes si no ha sido creada ya
    Id_Cola_Mensajes = msgget (Clave1, 0660 | IPC_CREAT);
    if (Id_Cola_Mensajes == -1)
    {
        printf("Error al obtener identificador para cola mensajes");
        return(0);
    }

    while(1)
    {

        // Recibimos el mensaje por la cola de mensajes
        // Será un mensaje de tipo 1
        msgrcv (Id_Cola_Mensajes, (struct msgbuf *)&Un_Mensaje,sizeof(Un_Mensaje.Dato_Numerico) + sizeof(Un_Mensaje.Mensaje), 1, 0);

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

        //Escribimos en el socket
        sockWrite=write(sockfd, Un_Mensaje.Mensaje, sizeof(Un_Mensaje.Mensaje));
        //Comprobamos errores
        if (sockWrite < 0)
        {
            perror("writing on stream socket");
            exit(EXIT_FAILURE);
        }

        //Leemos el eco del socket
        sockRead=read(sockfd, Un_Mensaje.Mensaje, sizeof(Un_Mensaje.Mensaje));
        //Comprobamos errores
        if (sockRead < 0)
        {
            perror("reading stream message");
            exit(EXIT_FAILURE);
        }

        // Mensaje a enviar será de tipo 2
        Un_Mensaje.Id_Mensaje = 2;
        Un_Mensaje.Dato_Numerico = 13;

        // Enviamos por la cola de mensajes
        msgsnd (Id_Cola_Mensajes, (struct msgbuf *)&Un_Mensaje,sizeof(Un_Mensaje.Dato_Numerico)+sizeof(Un_Mensaje.Mensaje),IPC_NOWAIT);

        //Cerramos el socket
        cerrarSocket=close(sockfd);
        //Comprobamos errores
        if(cerrarSocket<0)
        {
            perror("close()");
            exit(EXIT_FAILURE);
        }
    }

    // Cerramos la cola de mensajes
    msgctl (Id_Cola_Mensajes, IPC_RMID, (struct msqid_ds *)NULL);

    exit(0);
}
