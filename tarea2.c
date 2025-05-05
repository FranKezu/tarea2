#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// gcc tarea2.c tdas/list.c tdas/map.c tdas/extra.c -o tarea2

typedef struct {
  char id[100];
  char artists[100];
  char album_name[100];
  char track_name[100];
  float tempo;
  char track_genre[100];
} Film;

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


void cargar_musica(Map *music_byid, Map *music_bygenres) {
  FILE *archivo = fopen("song_dataset.csv", "r");
  if (archivo == NULL) {
    perror(
        "Error al abrir el archivo"); 
    return;
  }
  
  char **campos;
  campos = leer_linea_csv(archivo, ',');

  long contador = 0;
  while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
    Film *song = (Film *)malloc(sizeof(Film));
    strncpy(song->id, campos[0], 99);
    song->id[99] = '\0';
    
    strncpy(song->artists, campos[2], 99);
    song->artists[99] = '\0';
    
    strncpy(song->album_name, campos[3], 99);
    song->album_name[99] = '\0';
    
    strncpy(song->track_name, campos[4], 99);
    song->track_name[99] = '\0';
    
    song->tempo = atof(campos[18]);
    
    strncpy(song->track_genre, campos[20], 99);
    song->track_genre[99] = '\0';
    
    map_insert(music_byid, song->id, song);
    MapPair *genre_pair = map_search(music_bygenres, song->track_genre);

    if (genre_pair == NULL) {
      
      List *new_list = list_create();
      list_pushBack(new_list, song);
      map_insert(music_bygenres, song->track_genre, new_list);
    } else {
      List *genre_list = (List *)genre_pair->value;
      list_pushBack(genre_list, song);
    }
    contador++;
    if (contador % 1000 == 0) {
      printf("Cargando canciones (%ld / 114001)\n", contador);
    }
  }
  fclose(archivo); 
  printf("Canciones cargadas exitosamente (%ld)\n", contador);
  printf("Presione una tecla para continuar...\n");
  while (getchar() != '\n');
  getchar();
}

/**
 * Busca y muestra la información de una película por su ID en un mapa.
 */
/*
void buscar_por_id(Map *music_byid) {
  char id[10]; // Buffer para almacenar el ID de la película

  // Solicita al usuario el ID de la película
  printf("Ingrese el id de la película: ");
  scanf("%s", id); // Lee el ID del teclado

  // Busca el par clave-valor en el mapa usando el ID proporcionado
  MapPair *pair = map_search(music_byid, id);

  // Si se encontró el par clave-valor, se extrae y muestra la información de la
  // película
  if (pair != NULL) {
    Film *peli =
        pair->value; // Obtiene el puntero a la estructura de la película
    // Muestra el título y el año de la película
    printf("Título: %s, Año: %d\n", peli->artists, peli->year);
  } else {
    // Si no se encuentra la película, informa al usuario
    printf("La película con id %s no existe\n", id);
  }
}

void buscar_por_genero(Map *music_bygenres) {
  char genero[100];

  // Solicita al usuario el ID de la película
  printf("Ingrese el género de la película: ");
  scanf("%s", genero); // Lee el ID del teclado

  MapPair *pair = map_search(music_bygenres, genero);
  
  if (pair != NULL) {
      List* pelis = pair->value;
      Film *peli = list_first(pelis);
      
      while (peli != NULL) {
        printf("ID: %s, Título: %s, Director: %s, Año: %d\n", peli->id, peli->artists,
           peli->director, peli->year);
        peli = list_next(pelis);
      }
  }
}
*/
int main() {
  char opcion; // Variable para almacenar una opción ingresada por el usuario
               // (sin uso en este fragmento)

  // Crea un mapa para almacenar películas, utilizando una función de
  // comparación que trabaja con claves de tipo string.
  Map *music_byid = map_create(is_equal_str);
  Map *music_bygenres = map_create(is_equal_str);

  // Recuerda usar un mapa por criterio de búsqueda

  do {
    mostrarMenuPrincipal();
    printf("Ingrese su opción: ");
    scanf(" %c", &opcion);

    switch (opcion) {
    case '1':
      cargar_musica(music_byid, music_bygenres);
      break;
    case '2':
      //buscar_por_id(music_byid);
      break;
    case '3':
      break;
    case '4':
      //buscar_por_genero(music_bygenres);
      break;
    case '5':
      break;
    case '6':
      break;
    case '7':
      break;
    }
    presioneTeclaParaContinuar();

  } while (opcion != '8');

  return 0;
}