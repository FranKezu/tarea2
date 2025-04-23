#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char id[100];
  char artists[100];
  char album_name[100];
  char track_name[100];
  float tempo;
  char track_genre[100];
} Track;


// Menú principal
void mostrarMenuPrincipal() {
  limpiarPantalla();
  puts("========================================");
  puts("     Base de Datos de Canciones");
  puts("========================================");

  puts("1) Cargar canciones");
  puts("2) Buscar por género");
  puts("3) Buscar por artista");
  puts("4) Buscar por tempo");
  puts("5) Crear lista de reproducción");
  puts("6) Agregar canción a la lista");
  puts("7) Mostrar canciones de la lista");
  puts("8) Salir");
}

/**
 * Compara dos claves de tipo string para determinar si son iguales.
 * Esta función se utiliza para inicializar mapas con claves de tipo string.
 *
 * @param key1 Primer puntero a la clave string.
 * @param key2 Segundo puntero a la clave string.
 * @return Retorna 1 si las claves son iguales, 0 de lo contrario.
 */
int is_equal_str(void *key1, void *key2) {
  return strcmp((char *)key1, (char *)key2) == 0;
}

/**
 * Compara dos claves de tipo entero para determinar si son iguales.
 * Esta función se utiliza para inicializar mapas con claves de tipo entero.
 *
 * @param key1 Primer puntero a la clave entera.
 * @param key2 Segundo puntero a la clave entera.
 * @return Retorna 1 si las claves son iguales, 0 de lo contrario.
 */
int is_equal_int(void *key1, void *key2) {
  return *(int *)key1 == *(int *)key2; // Compara valores enteros directamente
}
void cargar_canciones(Map *tracks_by_id, Map *tracks_by_genre) {
  FILE *archivo = fopen("song_dataset.csv", "r");
  if (archivo == NULL) {
    perror(
        "Error al abrir el archivo");
    return;
  }

  char **linea;
  linea = leer_linea_csv(archivo, ',');

  while ((linea = leer_linea_csv(archivo, ',')) != NULL) {
    Track *song = (Track *)malloc(sizeof(Track));

    strcpy(song->id, linea[0]);
    strcpy(song->artists, linea[2]);
    strcpy(song->album_name, linea[3]);
    strcpy(song->track_name, linea[4]);
    song->tempo = atof(linea[18]);
    strcpy(song->track_genre, linea[20]);
  }
  fclose(archivo);

  map_insert(tracks_by_id, song->id, song);

  // Insertar en mapa por género (asumiendo un solo género por ahora)
  map_insert(tracks_by_genre, song->track_genre, song);
}

fclose(archivo);

// Mostrar canciones cargadas para depuración
printf("Canciones cargadas:\n");
MapPair *pair = map_first(tracks_by_id);
while (pair != NULL) {
  Track *song = (Track *)pair->value;
  printf("ID: %s, Track: %s, Artists: %s, Album: %s, Tempo: %.3f, Género: %s\n",
         song->id, song->track_name, song->artists, song->album_name, song->tempo, song->track_genre);
  pair = map_next(tracks_by_id);

}

int main(){
  char opcion;
  Map *tracks_by_id = map_create(is_equal_str);
  Map *tracks_by_genre = map_create(is_equal_str);

  do {
    mostrarMenuPrincipal();
    printf("Ingrese su opción: ");
    scanf(" %c", &opcion);

    switch (opcion) {
    case '1':
      cargar_canciones(tracks_by_id, tracks_by_genre);
      break;
    case '2':
      printf("Buscar por género:\n");
      break;
    case '3':
      printf("Buscar por artista:\n");
      break;
    case '4':
      printf("Buscar por tempo:\n");
      break;
    case '5':
      printf("Crear lista de reproducción:\n");
      break;
    case '6':
      printf("Agregar canción a la lista:\n");
      break;
    case '7':
      printf("Mostrar canciones de la lista:\n");
      break;
    case '8':
      printf("Saliendo...\n");
      break;
    default:
      printf("Opción inválida.\n");
    }
    presioneTeclaParaContinuar();

  } while (opcion != '8');
  return 0;
}