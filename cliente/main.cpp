#include <stdio.h>
#include <windows.h>
#include <winsock.h>

void initWinSock(WSADATA & WsaData);
void initSock(SOCKET & Sock);
void initSockAddr(SOCKADDR_IN & DireccionServer);
void connectSock(SOCKET & Sock, SOCKADDR_IN & DireccionServer);
void mostrarMenu();
void enviarSolicitud(SOCKET sock, int opcion, unsigned tam);

int main(){
    WSADATA WsaData;
    SOCKET Sock;
    SOCKADDR_IN DireccionServer;

    initWinSock(WsaData);
    initSock(Sock);
    initSockAddr(DireccionServer);
    connectSock(Sock, DireccionServer);

    int opcion;
    unsigned tam;
    char respuesta[1024] = {0};

    while (1) {
        mostrarMenu();
        printf("Ingresa tu opción: ");
        scanf("%d", &opcion);

        if (opcion == 3) {
            printf("Saliendo...\n");
            break;
        }
        if (opcion == 1) {
            printf("Ingresa el tamaño de la contraseña (8-50): ");
        } else
            if (opcion ==2){
            printf("Ingresa el tamaño del usuario (5-20): ");
            }else{

            printf("Opción inválida.\n");
            continue;
            }

        scanf("%u", &tam);


        enviarSolicitud(Sock, opcion, tam);
        memset(respuesta, 0, sizeof(respuesta));

        int bytes = recv(Sock, respuesta, sizeof(respuesta) - 1, 0);
        if (bytes > 0){
            printf("Respuesta del servidor: %s\n", respuesta);
            printf("\n");
        } else {
            printf("Error al recibir respuesta del servidor.\n");
        }
    }
    closesocket (Sock);
    WSACleanup();

    return 0;
}


void initWinSock(WSADATA &wsaData){
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Error al inicializar Winsock. codigo: %d\n", WSAGetLastError());
        exit(1);
    }
}

void initSock(SOCKET &Sock){
    Sock = socket (AF_INET, SOCK_STREAM, 0);
    if (Sock == INVALID_SOCKET){
        printf("Error al inicializar Socket. codigo: %d\n", WSAGetLastError());
        WSACleanup();
        exit(1);
    }
}

void initSockAddr(SOCKADDR_IN &DireccionServer){
    memset (&DireccionServer, 0, sizeof (DireccionServer));
    DireccionServer.sin_family = AF_INET;
    DireccionServer.sin_addr.S_un.S_un_b.s_b1 = 127;
    DireccionServer.sin_addr.S_un.S_un_b.s_b2 = 0;
    DireccionServer.sin_addr.S_un.S_un_b.s_b3 = 0;
    DireccionServer.sin_addr.S_un.S_un_b.s_b4 = 1;
    DireccionServer.sin_port = htons(5000);
}

void connectSock(SOCKET& Sock, SOCKADDR_IN& DireccionServer){
    int iResult = connect(Sock, (SOCKADDR *) & DireccionServer, sizeof (DireccionServer));
    if (iResult == SOCKET_ERROR){
        printf("No se puede conectar. codigo: %d\n", WSAGetLastError());
        closesocket(Sock);
        WSACleanup();
        exit(1);
    }
}

void mostrarMenu() {
    printf("Selecciona una opción:\n");
    printf("1. Generar contraseña\n");
    printf("2. Generar usuario\n");
    printf("3. Salir\n");
}

void enviarSolicitud(SOCKET sock, int opcion, unsigned tam) {
    int net_opcion = htonl(opcion);
    unsigned net_tam = htonl(tam);

    send(sock, (char*)&net_opcion, sizeof(net_opcion), 0);
    send(sock, (char*)&net_tam, sizeof(net_tam), 0);
}
