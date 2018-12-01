
//Cliente del robot sobre el protocolo TCP.

#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/unistd.h>
#include<netdb.h>

#define SERV_TCP_PORT 8500
#define SERV_HOST_ADDR "157.88.201.95"
#define MAXLINE 512

float ReverseFloat(float);

struct posicion
{
    float X;
    float Y;
};

struct datos_robot
{
    char modo;                      // ‘x’: ningún modo activo
                                    // ‘m’: manual sin HOME
                                    // ‘M’ manual con HOME
                                    // ‘D’: MDI
                                    // ‘A’: automático

    char motor_activo;              // ‘0’: ningún motor activo
                                    // ‘1/2/3’: activo el motor 1/2/3
                                    // ‘4’: todos los motores activos

    char tipo_interpolacion_G;      // ‘0’, ‘1’, ‘2’, ‘3’, ‘4’
    float vel_F;                    // velocidad lineal
    float radio_giro_R;             // radio
    struct posicion pos_XY_final;   // posición final de la trayectoria actual
    struct posicion pos_XY_cmd;     // posición comandada (o interpolada)
    struct posicion pos_XY_real;    // posición real
};

main(int argc, char *argv[])
{
    //Variables de programa

    int sockbn, sockfd, sockConect, sockWrite, sockRead, cerrarSocket;
    struct sockaddr_in serv_addr;
    struct datos_robot rec;

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

    //Leemos los datos del socket
    sockRead=read(sockfd, &rec, sizeof(rec));
    //Comprobamos errores
    if (sockRead < 0)
    {
        perror("reading stream message");
        exit(EXIT_FAILURE);
    }

    //Imprimimos los datos recibido.
    printf("\nDATOS RECIBIDOS: \n\n");
    printf("Modo de funcionamiento: ");
    switch (rec.modo)
    {
        case 'x':
            printf("Ningun modo activo.\n");
            break;
        case 'm':
            printf("Manual sin HOME.\n");
            break;
        case 'M':
            printf("Manual con HOME.\n");
            break;
        case 'D':
            printf("MDI.\n");
            break;
        case 'A':
            printf("Automatico.\n");
            break;
    }

    printf("\nMotores activos: ");
    switch (rec.motor_activo)
    {
        case '0':
            printf("Ningun motor activo.\n");
            break;
        case '1':
            printf("Activo el motor 1.\n");
            break;
        case '2':
            printf("Activo el motor 2.\n");
            break;
        case '3':
            printf("Activo el motor 3.\n");
            break;
        case '4':
            printf("Todos los motores activos.\n");
            break;
    }

    printf("\nTipo interpolacion G: %c\n", rec.tipo_interpolacion_G);

    // Cambiamos el formato de los floats (LD-BE)

    rec.vel_F=ReverseFloat(rec.vel_F); // Formato de la velocidad lineal
    rec.radio_giro_R=ReverseFloat(rec.radio_giro_R); // Formato del radio
    rec.pos_XY_final.X=ReverseFloat(rec.pos_XY_final.X); // Formato de la posicion final
    rec.pos_XY_final.Y=ReverseFloat(rec.pos_XY_final.Y);
    rec.pos_XY_cmd.X=ReverseFloat(rec.pos_XY_cmd.X); // Formato de la posicion interpolada
    rec.pos_XY_cmd.Y=ReverseFloat(rec.pos_XY_cmd.Y);
    rec.pos_XY_real.X=ReverseFloat(rec.pos_XY_real.X); // Formato de la posicion real
    rec.pos_XY_real.Y=ReverseFloat(rec.pos_XY_real.Y);

    // Imprimimos los floats correctamente

    printf("\nVelocidad: %f\n", rec.vel_F);
    printf("\nRadio de giro: %f\n", rec.radio_giro_R);
    printf("\nPosicion final (X,Y): (%f,%f)\n", rec.pos_XY_final.X,rec.pos_XY_final.Y);
    printf("\nPosicion interpolada (X,Y): (%f,%f)\n", rec.pos_XY_cmd.X,rec.pos_XY_cmd.Y);
    printf("\nPosicion real (X,Y): (%f,%f)\n\n", rec.pos_XY_real.X,rec.pos_XY_real.Y);

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

float ReverseFloat(float inFloat)
{
   float retVal;
   char *floatToConvert = ( char* ) & inFloat;
   char *returnFloat = ( char* ) & retVal;

   // swap the bytes into a temporary buffer
   returnFloat[0] = floatToConvert[3];
   returnFloat[1] = floatToConvert[2];
   returnFloat[2] = floatToConvert[1];
   returnFloat[3] = floatToConvert[0];

   return retVal;
}
