#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define RESET_COLOR   "\033[0m"
#define LIGHT_RED     "\033[1;31m"
#define LIGHT_GREEN   "\033[1;32m"
#define LIGHT_YELLOW  "\033[1;33m"
#define LIGHT_BLUE    "\033[1;34m"
#define LIGHT_CYAN    "\033[1;36m"
#define LIGHT_WHITE   "\033[1;37m"

// gcc tarea2.c tdas/list.c tdas/map.c tdas/extra.c -o tarea2
// $OutputEncoding = [Console]::OutputEncoding = [Text.UTF8Encoding]::new()

typedef struct {
  char id[100];
  List *artists;
  char album_name[100];
  char track_name[100];
  float tempo;
  char track_genre[100];
} Song;

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


void cargar_musica(Map *music_byid, Map *music_bygenres, Map *music_byartist) {
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
    Song *song = (Song *)malloc(sizeof(Song));
    strncpy(song->id, campos[0], 99);
    song->id[99] = '\0';

    if (campos[2] == NULL || strlen(campos[2]) == 0) {
      song->artists = list_create();
    } else {
      song->artists = split_string(campos[2], ";");
    }
    
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

    char *artista = (char *)list_first(song->artists);

    while (artista != NULL) {
      MapPair *artist_pair = map_search(music_byartist, artista);
      if (artist_pair == NULL) {
        List *aux_list = list_create();
        if (aux_list == NULL) {
          printf("Error: No se pudo crear una lista para el artista %s.\n", artista);
          free(song);
          fclose(archivo);
          printf("Presione una tecla para continuar...\n");
          presioneTeclaParaContinuar();
          return;
        }
        list_pushBack(aux_list, song);
        char *artista_copy = strdup(artista);
        map_insert(music_byartist, artista_copy, aux_list);
      } else {
        List *artist_list = (List *)artist_pair->value;
        list_pushBack(artist_list, song);
      }
      artista = (char *)list_next(song->artists);
    }

    contador++;
    if (contador % 1000 == 0) {
      printf("Cargando canciones (%ld / 114000)\n", contador);
    }

    //if(contador == 10000) break;
  }
  fclose(archivo); 
  printf("\nCanciones cargadas exitosamente!\n\n");
}

void buscar_por_genero(Map *music_bygenres) {

  if(map_first(music_bygenres) == NULL){
      printf("¡El programa no puede funcionar si no se han cargado canciones!\n\n");
      return;
  }    
  char *lista_generos[114] = {
    "acoustic", "afrobeat", "alt-rock", "alternative", "ambient",
    "anime", "black-metal", "bluegrass", "blues", "brazil",
    "breakbeat", "british", "cantopop", "chicago-house", "children",
    "chill", "classical", "club", "comedy", "country",
    "dance", "dancehall", "death-metal", "deep-house", "detroit-techno",
    "disco", "disney", "drum-and-bass", "dub", "dubstep",
    "edm", "electro", "electronic", "emo", "folk",
    "forro", "french", "funk", "garage", "german",
    "gospel", "goth", "grindcore", "groove", "grunge",
    "guitar", "happy", "hard-rock", "hardcore", "hardstyle",
    "heavy-metal", "hip-hop", "honky-tonk", "house", "idm",
    "indian", "indie", "indie-pop", "industrial", "iranian",
    "j-dance", "j-idol", "j-pop", "j-rock", "jazz",
    "k-pop", "kids", "latin", "latino", "malay",
    "mandopop", "metal", "metalcore", "minimal-techno", "mpb",
    "new-age", "opera", "pagode", "party", "piano",
    "pop", "pop-film", "power-pop", "progressive-house", "psych-rock",
    "punk", "punk-rock", "r-n-b", "reggae", "reggaeton",
    "rock", "rock-n-roll", "rockabilly", "romance", "sad",
    "salsa", "samba", "sertanejo", "show-tunes", "singer-songwriter",
    "ska", "sleep", "songwriter", "soul", "spanish",
    "study", "swedish", "synth-pop", "tango", "techno",
    "trance", "trip-hop", "turkish", "world-music"};

  char genero[100];
  
  printf("\n─────────────────────────────────────────────────────────────────────────\n");
  printf("BÚSQUEDA POR GÉNERO MUSICAL\n");
  printf("─────────────────────────────────────────────────────────────────────────\n");
  printf("Ingrese el género de la canción: ");
  scanf("%s", genero);

  MapPair *pair = map_search(music_bygenres, genero);

  while(pair == NULL){
    printf("\nEl género ingresado no es válido. ¡Por favor ingrese uno de los siguientes géneros!\n\n");

    printf("Lista de géneros musicales disponibles:\n");
    for (int i = 0; i < 114; i++) {
      printf("- %s", lista_generos[i]);
      if (i % 14 == 0 && i != 0)
        printf("\n");
      else 
        printf(" ");
    }
    printf("\n\n");
    printf("Ingrese el género de la canción: ");
    scanf("%s", genero);
    pair = map_search(music_bygenres, genero);
  }
  
  List* canciones = pair->value;
  Song *cancion = list_first(canciones);
      
  while (cancion != NULL) {
    printf("ID: %s, Artista(s): ", cancion->id);

    char *current = list_first(cancion->artists);
    printf("%s", current);
    
    while(current != NULL){
      current = list_next(cancion->artists);
      if(current != NULL) printf(", %s", current);
    }

    printf(" - Album: %s - Track: %s - Tempo: %f - Track Genre: %s\n", cancion->album_name, 
      cancion->track_name, cancion->tempo, cancion->track_genre);
    cancion = list_next(canciones);
  }
}

void buscar_por_artista(Map *music_byartist) {
  char artista[100];
  
  printf("\n─────────────────────────────────────────────────────────────────────────\n");
  printf("BÚSQUEDA POR ARTISTA\n");
  printf("─────────────────────────────────────────────────────────────────────────\n");
  printf("Ingrese el artista que desea buscar: ");
  scanf(" %[^\n]s", artista);

  MapPair *pair = map_search(music_byartist, artista);

  while(pair == NULL){
    printf("\nEl artista ingresado no se encontró.\n\n");
    printf("Ingrese el artista que desea buscar: ");
    scanf(" %[^\n]s", artista);
    pair = map_search(music_byartist, artista);
  }
  
  List* canciones = pair->value;
  Song *cancion = list_first(canciones);
      
  while (cancion != NULL) {
    printf("ID: %s, Artista(s): ", cancion->id);

    char *current = list_first(cancion->artists);
    printf("%s", current);
    
    while(current != NULL){
      current = list_next(cancion->artists);
      if(current != NULL) printf(", %s", current);
    }

    printf(" - Album: %s - Track: %s - Tempo: %f - Track Genre: %s\n", cancion->album_name, 
      cancion->track_name, cancion->tempo, cancion->track_genre);
    cancion = list_next(canciones);
  }
}

int main() {
  char opcion;
  Map *music_byid = map_create(is_equal_str);
  Map *music_bygenres = map_create(is_equal_str);
  Map *music_byartist = map_create(is_equal_str);

  bool cargadas = false;
  do {
    mostrarMenuPrincipal();
    printf("Ingrese su opción: ");
    scanf(" %c", &opcion);

    switch (opcion) {
    case '1':
      if(cargadas) printf("¡Las canciones ya se cargaron!\n\n");
      else {
        cargadas = true;
        cargar_musica(music_byid, music_bygenres, music_byartist);
      }  
      break;
    case '2':
      buscar_por_genero(music_bygenres);
      break;
    case '3':
      buscar_por_artista(music_byartist);
      break;
    case '4':
      //buscar_por_tempo(music_byid);
      break;
    case '5':
      break;
    case '6':
      break;
    case '7':
      break;
    case '8':
      puts("Saliendo del programa...");
      break;
    default:
      puts("Opción no válida. Por favor, intente de nuevo.");
    }
    presioneTeclaParaContinuar();

  } while (opcion != '8');

  return 0;
}