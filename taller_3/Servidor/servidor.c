/*librerias del servidor*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

/*parametros del servidor*/
#define PORT 8080
#define MAX_BUFFER_SIZE 1024
#define BACKLOG 5

/* strings / errors*/
#include <errno.h>
#include <stdio.h> 
#include <string.h> 

typedef struct {
    char carrera[MAX_BUFFER_SIZE];
    char codigo[MAX_BUFFER_SIZE];
} Carrera;

typedef struct {
    char facultad[MAX_BUFFER_SIZE];
    char codigo[MAX_BUFFER_SIZE];
} Facultad;

void manejo_peticion_cliente(int socketCliente);
void verificar_matricula(int socketCliente);

int main() {
    int socketServidor, socketCliente;
    struct sockaddr_in direccionServidor, direccionCliente;
    socklen_t tamañoDireccion = sizeof(struct sockaddr_in);

    /*creacion del socket*/
    socketServidor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketServidor == -1) {
        fprintf(stderr, "[SERVER-error]: socket creation failed. %d: %s \n", errno, strerror( errno ));
        return -1;
    }

    else {
        printf("[SERVER]: Socket successfully created..\n"); 
    }

    /* formateando estructura con ceros */
    memset(&direccionServidor, 0, sizeof(direccionServidor));

    /*inicializando la direccion de la estructura de direccionamiento del servidor*/
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_addr.s_addr = INADDR_ANY;
    direccionServidor.sin_port = htons(PORT);

    /*bind socket*/
    if ((bind(socketServidor, (struct sockaddr *)&direccionServidor, sizeof(direccionServidor))) != 0) { 
        fprintf(stderr, "[SERVER-error]: socket bind failed. %d: %s \n", errno, strerror( errno ));
        return -1;
    } 

    else {
        printf("[SERVER]: Socket successfully binded \n");
    }

    /* Listen */
    if ((listen(socketServidor, BACKLOG)) != 0) { 
        fprintf(stderr, "[SERVER-error]: socket listen failed. %d: %s \n", errno, strerror( errno ));
        return -1;
    } 

    else {
        printf("[SERVER]: Listening on SERV_PORT %d \n\n", ntohs(direccionServidor.sin_port) ); 
    }

    while (1) {
        if ((socketCliente = accept(socketServidor, (struct sockaddr*)&direccionCliente, &tamañoDireccion)) == -1) {
            fprintf(stderr, "[SERVER-error]: error accepting connection. %d: %s \n", errno, strerror( errno ));
            continue;
        }
        manejo_peticion_cliente(socketCliente);
    }

    close(socketServidor);
    return 0;
}

void manejo_peticion_cliente(int socketCliente) {
    char buffer[MAX_BUFFER_SIZE];
    FILE *archivoCarreras = fopen("/home/savier018/c-programas/archivos/carrera.txt", "r");
    FILE *archivoFacultades = fopen("/home/savier018/c-programas/archivos/facultad.txt", "r");
    int lecturaBuffer = 0;

    if (archivoCarreras == NULL) {
        perror("Error opening codes file");
        exit(EXIT_FAILURE);
    }

    lecturaBuffer = read(socketCliente, buffer, sizeof(buffer));

    if (lecturaBuffer == -1) {
        fprintf(stderr, "[SERVER-error]: client socket cannot be read. %d: %s \n", errno, strerror( errno ));
    }
    else if (lecturaBuffer == 0) { /* if length is 0 client socket closed, then exit */             
        printf("[SERVER]: client socket closed \n\n");
        close(socketCliente);
    }

    while (lecturaBuffer > 0) {
        buffer[lecturaBuffer] = '\0';
        Carrera inputCliente;
        if (sscanf(buffer, "%s %s", inputCliente.carrera, inputCliente.codigo) != 2) {
            write(socketCliente, "[SERVER]: format error\n", sizeof("[SERVER]: format error\n"));
            continue;
        }

        fseek(archivoCarreras, 0, SEEK_SET);
        int encontrado = 0;
        char bufferCarrera[MAX_BUFFER_SIZE];
        char bufferCodigo[MAX_BUFFER_SIZE];

        while (fscanf(archivoCarreras, "%s %s", bufferCarrera, bufferCodigo) == 2) {
            if (strcmp(inputCliente.carrera, bufferCarrera) == 0 && strcmp(inputCliente.codigo, bufferCodigo) == 0) {
                encontrado = 1;
                break;
            }
        }

        if (encontrado) {
            write(socketCliente, "[SERVER]: career and code were found\n", strlen("[SERVER]: career and code were found\n"));
            verificar_matricula(socketCliente);
            break;
        } else {
            write(socketCliente, "[SERVER]: career and code were not found\n", strlen("[SERVER]: career and code were not found\n"));

            fseek(archivoFacultades, 0, SEEK_SET);
            int encontradoFacultad = 0;
            char bufferFacultad[MAX_BUFFER_SIZE];
            char bufferCodigoFacultad[MAX_BUFFER_SIZE];

            while (fscanf(archivoFacultades, "%s %s", bufferFacultad, bufferCodigoFacultad) == 2) {
                if (strcmp(inputCliente.carrera, bufferFacultad) == 0 && strcmp(inputCliente.codigo, bufferCodigoFacultad) == 0) {
                    encontradoFacultad = 1;
                    break;
                }
            }

            if (encontradoFacultad) {
                write(socketCliente, "[SERVER]: faculty and code were found\n", strlen("[SERVER]: faculty and code were found\n"));
                verificar_matricula(socketCliente);
            } else {
                write(socketCliente, "[SERVER]: faculty and code were not found\n", strlen("[SERVER]: faculty and code were not found\n"));
            }
            break;
        }

    }

    fclose(archivoCarreras);
    fclose(archivoFacultades);

}

void verificar_matricula(int socketCliente) {
    char buffer[MAX_BUFFER_SIZE];
    FILE *archivoEstudiantes = fopen("/home/savier018/c-programas/archivos/estudiantes.txt", "r");
    int lecturaBuffer = 0;

    if (archivoEstudiantes == NULL) {
        perror("Error opening codes file");
        exit(EXIT_FAILURE);
    }

    lecturaBuffer = read(socketCliente, buffer, sizeof(buffer));

    if (lecturaBuffer == -1) {
        fprintf(stderr, "[SERVER-error]: client socket cannot be read. %d: %s \n", errno, strerror( errno ));
    }
    else if (lecturaBuffer == 0) { /* if length is 0 client socket closed, then exit */             
        printf("[SERVER]: client socket closed \n\n");
        close(socketCliente);
    }
    
}
