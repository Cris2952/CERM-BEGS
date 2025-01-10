#include <stdio.h>
#include <string.h>
#include <mosquitto.h>
#include <mariadb/mysql.h>
#include <unistd.h>  // Para usar sleep()

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message) {
    printf("Mensaje recibido: %s\n", (char*)message->payload);

    // Conexión a la base de datos MariaDB
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "Error al inicializar la conexión a MariaDB: %s\n", mysql_error(conn));
        return;
    }

    if (mysql_real_connect(conn, "localhost", "root", "tesoem", "sistema_productor_consumidor", 0, NULL, 0) == NULL) {
        fprintf(stderr, "Error de conexión a la base de datos: %s\n", mysql_error(conn));
        mysql_close(conn);
        return;
    } else {
        printf("Conexión a la base de datos establecida.\n");
    }

    // Insertar el mensaje en la base de datos (simplificado)
    char query[1024];
    snprintf(query, sizeof(query), "INSERT INTO mensajes (matricula, nombre, primer_apellido, segundo_apellido, materia, calificacion, carrera) VALUES (12345, 'Juan', 'Pérez', 'González', 'Matemáticas', '9.5', 'Ingeniería')");

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Error en la inserción: %s\n", mysql_error(conn));
    } else {
        printf("Mensaje insertado en la base de datos.\n");
    }

    mysql_close(conn);
}

int main() {
    struct mosquitto *mosq;
    int rc;

    mosquitto_lib_init();
    mosq = mosquitto_new(NULL, true, NULL);

    rc = mosquitto_connect(mosq, "localhost", 1883, 60);
    if (rc != MOSQ_ERR_SUCCESS) {
        printf("Error de conexión: %s\n", mosquitto_strerror(rc));
        return 1;
    }

    // Suscribirse al topic "estudiantes/info"
    mosquitto_subscribe(mosq, NULL, "estudiantes/info", 0);

    mosquitto_message_callback_set(mosq, on_message);

    // Usar un bucle no bloqueante con sleep para evitar bloqueo total
    while (1) {
        mosquitto_loop(mosq, -1, 1);  // Procesa mensajes
        sleep(1);  // Pausa de 1 segundo para evitar bloqueo completo
    }

    mosquitto_lib_cleanup();

    return 0;
}
