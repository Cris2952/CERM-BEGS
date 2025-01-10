#include <stdio.h>
#include <string.h>
#include <mosquitto.h>

struct mensaje {
    int matricula;
    char *nombre;
    char *primer_apellido;
    char *segundo_apellido;
    char *materia;
    char *calificacion;
    char *carrera;
};

void enviar_mensaje(struct mosquitto *mosq, struct mensaje msg) {
    char payload[1024];
    snprintf(payload, sizeof(payload), "{\"matricula\": %d, \"nombre\": \"%s\", \"primer_apellido\": \"%s\", \"segundo_apellido\": \"%s\", \"materia\": \"%s\", \"calificacion\": \"%s\", \"carrera\": \"%s\"}",
             msg.matricula, msg.nombre, msg.primer_apellido, msg.segundo_apellido, msg.materia, msg.calificacion, msg.carrera);
    
    mosquitto_publish(mosq, NULL, "estudiantes/info", strlen(payload), payload, 0, false);
    printf("Mensaje enviado: %s\n", payload);
}

int main() {
    struct mosquitto *mosq;
    int rc;

    mosquitto_lib_init();
    mosq = mosquitto_new(NULL, true, NULL);

    rc = mosquitto_connect(mosq, "localhost", 1883, 60);
    if(rc != MOSQ_ERR_SUCCESS){
        printf("Error de conexión: %s\n", mosquitto_strerror(rc));
        return 1;
    }

    // Mensaje predeterminado
    struct mensaje msg;
    msg.matricula = 12345;
    msg.nombre = "Juan";
    msg.primer_apellido = "Pérez";
    msg.segundo_apellido = "González";
    msg.materia = "Matemáticas";
    msg.calificacion = "9.5";
    msg.carrera = "Ingeniería";

    // Enviar el mensaje predeterminado
    enviar_mensaje(mosq, msg);

    mosquitto_disconnect(mosq);
    mosquitto_lib_cleanup();

    return 0;
}

