#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>

void initWinSock(WSADATA *wsaData);
void initSock(SOCKET *Sock);
void initSockAddr(SOCKADDR_IN *DireccionLocal);
void asociarDireccion(SOCKET sockEscucha, SOCKADDR_IN *DireccionLocal);
void iniciarEscucha(SOCKET sockEscucha, int backlog);
void aceptarConexion(SOCKET sockEscucha, SOCKET *sockConexion);

void generarContrasena(char* cadena, int longitud);
void generarCadenaAlternada(char* cadena, int longitud);

int main(){
    srand(time(0));

    WSADATA wsaData;
    SOCKET sockEscucha;
    SOCKADDR_IN DireccionServer;

    initWinSock(&wsaData);
    initSock(&sockEscucha);
    initSockAddr(&DireccionServer);


    asociarDireccion(sockEscucha, &DireccionServer);
    iniciarEscucha(sockEscucha, 5);


    printf("Esperando conexiones..\n");


    while (1) {
        SOCKET sockConexion;
        aceptarConexion(sockEscucha, &sockConexion);

        printf("Cliente conectado.\n");

        while (1) {
            int opcion;
            unsigned tam;
            char respuesta[1024] = {0};


            int bytesRecibidos = recv(sockConexion, (char*)&opcion, sizeof(opcion), 0);
            if (bytesRecibidos <= 0) {
                printf("Error al recibir la opción del cliente o conexión cerrada.\n");
                closesocket(sockConexion);
                break;
            }

            bytesRecibidos = recv(sockConexion, (char*)&tam, sizeof(tam), 0);
            if (bytesRecibidos <= 0) {
                printf("Error al recibir el tamaño.\n");
                closesocket(sockConexion);
                break;
            }

                opcion = ntohl(opcion);
                tam = ntohl(tam);

            if (opcion == 2) {
                if (tam < 5 || tam > 15) {
                    const char* respuesta1 = "Tamaño inválido para nombre de usuario";
                    send(sockConexion, respuesta1, strlen(respuesta1), 0);
                } else {
                    char cadena[101];
                    memset(cadena, 0, sizeof(cadena));
                    generarCadenaAlternada(cadena, tam);
                    send(sockConexion, cadena, strlen(cadena), 0);
                }
            } else if (opcion == 1) {
                if (tam < 8 || tam > 50) {
                    const char* respuesta2 = "Tamaño inválido para contraseña";
                    send(sockConexion, respuesta2, strlen(respuesta2), 0);
                } else {
                    char cadena[51];
                    memset(cadena, 0, sizeof(cadena));
                    generarContrasena(cadena, tam);
                    send(sockConexion, cadena, strlen(cadena), 0);
                }
            } else {
                const char* respuestaError = "Opción invalida";
                send(sockConexion, respuestaError, strlen(respuestaError), 0);
            }
        }

        closesocket(sockConexion);
        printf("Cliente desconectado.\n");
    }


    WSACleanup();

    return 0;
}

void initWinSock(WSADATA *wsaData) {
    if (WSAStartup(MAKEWORD(2, 2), wsaData) != 0) {
        printf("Error al inicializar Winsock. codigo: %d\n", WSAGetLastError());
        exit(1);
    }
}

void initSock(SOCKET *Sock) {
    *Sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*Sock == INVALID_SOCKET) {
        printf("Error al inicializar Socket. codigo: %d\n", WSAGetLastError());
        WSACleanup();
        exit(1);
    }
}

void initSockAddr(SOCKADDR_IN *DireccionLocal) {
    memset(DireccionLocal, 0, sizeof(*DireccionLocal));
    DireccionLocal->sin_family = AF_INET;
    DireccionLocal->sin_port = htons(5000);
    DireccionLocal->sin_addr.s_addr = INADDR_ANY;
}

void asociarDireccion(SOCKET sockEscucha, SOCKADDR_IN *DireccionLocal) {
    if (bind(sockEscucha, (SOCKADDR*)DireccionLocal, sizeof(*DireccionLocal)) == SOCKET_ERROR) {
        printf("Error al asociar la dirección. codigo: %d\n", WSAGetLastError());
        closesocket(sockEscucha);
        WSACleanup();
        exit(1);
    }
}

void iniciarEscucha(SOCKET sockEscucha, int backlog) {
    if (listen(sockEscucha, backlog) == SOCKET_ERROR) {
        printf("Error al iniciar el socket de escucha. codigo: %d\n", WSAGetLastError());
        closesocket(sockEscucha);
        WSACleanup();
        exit(1);
    }
}

void aceptarConexion(SOCKET sockEscucha, SOCKET *sockConexion) {
    *sockConexion = accept(sockEscucha, NULL, NULL);
    if (*sockConexion == INVALID_SOCKET) {
        printf("Error al aceptar la conexión. codigo: %d\n", WSAGetLastError());
        closesocket(sockEscucha);
        WSACleanup();
        exit(1);
    }
}

void generarContrasena(char* cadena, int longitud) {
    char caracteres[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int totalCaracteres = sizeof(caracteres) - 1;

    for (int i = 0; i < longitud; i++) {
        cadena[i] = caracteres[rand() % totalCaracteres];
    }


}

void generarCadenaAlternada(char* cadena, int longitud) {
    char vocales[] = {'a', 'e', 'i', 'o', 'u'};
    char consonantes[] = {'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm','n', 'p', 'q', 'r', 's', 't', 'v', 'w', 'x', 'y', 'z'};

    int esVocal = rand() % 2;

    for (int i = 0; i < longitud; i++) {
        if (esVocal) {
            cadena[i] = vocales[rand() % sizeof(vocales)];

        } else {
            cadena[i] = consonantes[rand() % sizeof(consonantes)];

        }
        esVocal = !esVocal;
    }
}
