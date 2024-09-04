import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.util.Scanner;



public class Main {
    public static void mostrarMenu() {
        System.out.println("Selecciona una opción:");
        System.out.println("1. Generar contraseña");
        System.out.println("2. Generar usuario");
        System.out.println("3. Salir");
    }

    public static void enviarSolicitud(DataOutputStream out, int opcion, int tam) throws IOException {
        out.writeInt(opcion);
        out.writeInt(tam);
    }

    public static void main(String[] args) {


        String host = "127.0.0.1";
        int port = 5000;

        try (Socket socket = new Socket(host, port);
             DataOutputStream out = new DataOutputStream(socket.getOutputStream());
             DataInputStream in = new DataInputStream(socket.getInputStream());
             Scanner scanner = new Scanner(System.in)) {

            int opcion;
            int tam;
            byte[] buffer = new byte[1024];

            while (true) {
                mostrarMenu();
                System.out.print("Ingresa tu opción: ");
                opcion = scanner.nextInt();

                if (opcion == 3) {
                    System.out.println("Saliendo...");
                    break;
                }
                if (opcion == 1) {
                    System.out.print("Ingresa el tamaño de la contraseña (8-50): ");
                } else
                if (opcion ==2){
                    System.out.print("Ingresa el tamaño del usuario (5-15): ");
                }else{

                    System.out.println("Opción inválida.");
                    continue;
                }


                tam = scanner.nextInt();

                enviarSolicitud(out, opcion, tam);

                for (int i = 0; i < buffer.length; i++) {
                    buffer[i] = 0;
                }

                int bytesRecibidos = in.read(buffer);
                if (bytesRecibidos > 0) {
                    String respuesta = new String(buffer, 0, bytesRecibidos);
                    System.out.println("Respuesta del servidor: " + respuesta);
                } else {
                    System.out.println("Error al recibir respuesta del servidor.");
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
