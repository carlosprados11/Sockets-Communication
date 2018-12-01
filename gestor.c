
// GESTOR

#include<stdio.h>
#include<sys/types.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/ipc.h>
#include<sys/msg.h>

#define MAXTEXT 100

struct Mi_Tipo_Mensaje
{
 long Id_Mensaje;
 int Dato_Numerico;
 char Mensaje[MAXTEXT];
}Un_Mensaje;

main(int argc, char *argv[])
{
    //Variables de programa
    char Datos[MAXTEXT];

    char *devf;
    char fichero[MAXTEXT];

    // Solicito el nombre del fichero
    printf("\nNombre del fichero: ");
    gets(fichero);

    // Definimos archivo
    FILE *archivo;
    archivo = fopen(fichero,"r");
    *Datos="";

    // Cola de mensajes
    key_t Clave1;
    int Id_Cola_Mensajes;

    // Obtengo una clave para la cola de mensajes
    Clave1 = ftok("/bin/ls", 35);
    if (Clave1 == (key_t)-1)
    {
        printf("Error al obtener clave para cola mensajes");
        return(0);
    }

    // Se crea la cola de mensajes y se obtiene un identificador para ella.
    // El IPC_CREAT indica que cree la cola de mensajes si no lo está.
    // el 0660 son permisos de lectura y escritura para el usuario que lance
    // los procesos. Es importante el 0 delante para que se interprete en octal.

    Id_Cola_Mensajes = msgget (Clave1, 0660 | IPC_CREAT);
    if (Id_Cola_Mensajes == -1)
    {
        printf("Error al obtener identificador para cola mensajes");
        return(0);
    }

    printf("\nCONTENIDO DEL FICHERO SELECCIONADO: \n\n");

    while(fgets(Datos, MAXTEXT, (FILE*)archivo) != NULL)
    {
        // RELLENO EL MENSAJE "Fin"

        // El Id_Mensaje es un identificador del tipo de mensaje (para distinguir
        // el sentido del mensaje)
        // 1 = gestor a cliente_b
        // 2 = cliente_b a gestor
        // Dato_Numerico es un dato que se quiera pasar al otro proceso.
        // Mensaje es un texto que se quiera enviar

        Un_Mensaje.Id_Mensaje = 1;
        Un_Mensaje.Dato_Numerico = 29;
        strcpy(Un_Mensaje.Mensaje, Datos);

        // MANDAR

        // Se envia el mensaje. Los parámetros son:
        // - Id de la cola de mensajes.
        // - Dirección al mensaje, convirtiéndola en puntero a (struct msgbuf *)
        // - Tamaño total de los campos de datos de nuestro mensaje
        // - Unos flags. IPC_NOWAIT indica que si el mensaje no se puede enviar
        // (habitualmente porque la cola de mensajes esta llena), que no espere
        // y de un error. Si no se pone este flag, el programa queda bloqueado
        // hasta que se pueda enviar el mensaje.

        msgsnd (Id_Cola_Mensajes, (struct msgbuf *)&Un_Mensaje,sizeof(Un_Mensaje.Dato_Numerico)+sizeof(Un_Mensaje.Mensaje),IPC_NOWAIT);

        // RECIBIR

        // Se recibe un mensaje del otro proceso. Los parámetros son:
        // - Id de la cola de mensajes.
        // - Dirección del sitio en el que queremos recibir el mensaje,
        // convirtiéndolo en puntero a (struct msgbuf *).
        // - Tamaño máximo de nuestros campos de datos.
        // - Identificador del tipo de mensaje que queremos recibir. En este caso
        // se quiere un mensaje de tipo 2. Si ponemos tipo 1, se extrae el mensaje
        // que se acaba de enviar en la llamada anterior a msgsnd().
        // - flags. En este caso se quiere que el programa quede bloqueado hasta
        // que llegue un mensaje de tipo 2. Si se pone IPC_NOWAIT, se devolvería
        // un error en caso de que no haya mensaje de tipo 2 y el programa
        // continuaría ejecutándose.

        msgrcv (Id_Cola_Mensajes, (struct msgbuf *)&Un_Mensaje,sizeof(Un_Mensaje.Dato_Numerico) + sizeof(Un_Mensaje.Mensaje),2, 0);

        printf("%s",Un_Mensaje.Mensaje);

    }

    printf("\n");

    // Cerramos el fichero de texto
    fclose(archivo);

    // Se borra y cierra la cola de mensajes.
    // IPC_RMID indica que se quiere borrar. El puntero del final son datos
    // que se quieran pasar para otros comandos. IPC_RMID no necesita datos,
    // así que se pasa un puntero a NULL.
    msgctl (Id_Cola_Mensajes, IPC_RMID, (struct msqid_ds *)NULL);

    exit(0);
}
