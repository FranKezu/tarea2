#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <locale.h>

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
  puts("╔════════════════════════════════════════════╗");
  puts("║              Spotifind - Menú              ║");
  puts("╠════════════════════════════════════════════╣");
  puts("║  1) Cargar canciones                       ║");
  puts("║  2) Buscar por género                      ║");
  puts("║  3) Buscar por artista                     ║");
  puts("║  4) Buscar por tempo                       ║");
  puts("║  5) Crear lista de reproducción            ║");
  puts("║  6) Agregar canción a la lista             ║");
  puts("║  7) Mostrar canciones de la lista          ║");
  puts("║  8) Salir                                  ║");
  puts("╚════════════════════════════════════════════╝");
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
  return *(int *)key1 == *(int *)key2;
}

char *velocidad(float tempo){
  if(tempo < 80 ) return "Lentas";
  if (tempo <= 120) return "Moderadas";
  return "Rápidas";
}

void cargar_musica(Map *music_byid, Map *music_bygenres, Map *music_byartist, Map *music_bytempo) {
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

    //Mapa por género
    MapPair *genre_pair = map_search(music_bygenres, song->track_genre);
    if (genre_pair == NULL) {
      
      List *new_list = list_create();
      list_pushBack(new_list, song);
      map_insert(music_bygenres, song->track_genre, new_list);
    } else {
      List *genre_list = (List *)genre_pair->value;
      list_pushBack(genre_list, song);
    }

    //Mapa por artista
    char *artista = (char *)list_first(song->artists);
    while (artista != NULL) {
      MapPair *artist_pair = map_search(music_byartist, artista);
      if (artist_pair == NULL) {
        List *aux_list = list_create();
        list_pushBack(aux_list, song);
        char *artista_copy = strdup(artista);
        map_insert(music_byartist, artista_copy, aux_list);
      } else {
        List *artist_list = (List *)artist_pair->value;
        list_pushBack(artist_list, song);
      }
      artista = (char *)list_next(song->artists);
    }


    //Mapa por tempo

    MapPair *tempo_pair = map_search(music_bytempo, velocidad(song->tempo));
    if (tempo_pair == NULL) {
      
      List *new_list = list_create();
      list_pushBack(new_list, song);
      map_insert(music_bytempo, velocidad(song->tempo), new_list);
    } else {
      List *tempo_list = (List *)tempo_pair->value;
      list_pushBack(tempo_list, song);
    }

    contador++;
    if (contador % 1000 == 0) {
      printf("Cargando canciones (%ld / 114000)\n", contador);
    }

    if(contador == 10000) break;
  }
  fclose(archivo); 
  printf("\nCanciones cargadas exitosamente!\n\n");
}

void mostrar_canciones(MapPair *pair) {
  List *canciones = (List *)pair->value;
  printf("\nLista de canciones:\n");
  Song *cancion = (Song *)list_first(canciones);
  while (cancion != NULL) {
    printf("ID: %s | Artista(s): ", cancion->id);
    char *current = (char *)list_first(cancion->artists);
    if (current == NULL) {
      printf("Desconocido");
  } else {
    printf("%s", current);
    current = (char *)list_next(cancion->artists);
    while (current != NULL) {
      printf(", %s", current);
      current = (char *)list_next(cancion->artists);
      }
    }
  printf(" | Álbum: %s | Canción: %s | Tempo: %.0f | Género: %s\n",
  cancion->album_name, cancion->track_name, cancion->tempo, cancion->track_genre);
  cancion = (Song *)list_next(canciones);
  }
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

  mostrar_canciones(pair);
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
  
  mostrar_canciones(pair);
}

void buscar_por_tempo(Map *music_bytempo) {
  char tempo[100];
  
  printf("\n─────────────────────────────────────────────────────────────────────────\n");
  printf("BÚSQUEDA POR VELOCIDAD | (LENTAS - MODERADAS - RAPIDAS)\n");
  printf("─────────────────────────────────────────────────────────────────────────\n");
  printf("Ingrese el tempo que desea buscar: ");
  scanf(" %[^\n]s", tempo);

  MapPair *pair = map_search(music_bytempo, tempo);

  while(pair == NULL){
    printf("\nLa categoria de velocidad que busca no existe .\n\n");
    printf("Ingrese la velocidad que desea buscar (Lentas-Moderadas-Rápidas): ");
    scanf(" %[^\n]s", tempo);
    pair = map_search(music_bytempo, tempo);
  }
  
  mostrar_canciones(pair);
}

void crear_lista_repro(Map *listas_repro){
  char nombre[100];
  printf("Ingrese el nombre para su lista de reproducción: ");
  scanf(" %[^\n]s", nombre);
  MapPair *pair = map_search(listas_repro, nombre);

  if (pair == NULL) {
    List *new_list = list_create();
    char *nombre_copia = strdup(nombre);
    map_insert(listas_repro, nombre_copia, new_list);
    printf("\nLista de reproducción (%s) creada con éxito.\n", nombre_copia);
  } else {
    printf("\nYa existe una lista de reproducción con ese nombre, por favor intenta con otro.\n");
  }

}

void agregar_cancion(Map *listas_repro) {
  //printf("A qué lista quiere agregar la canción:\n");

  MapPair *pair = map_first(listas_repro);
  if (pair == NULL) {
      printf("No hay listas de reproducción disponibles.\n");
      return;
  }
  puts("Listas de reproducción creadas:");
  puts("  N° | Nombre                    | Canciones ");
  puts("-----+---------------------------+-----------");
  int contador = 1;
  while (pair != NULL) {
      printf(" %2d  | %-25s |     %3d\n", contador, (char *)pair->key, list_size(pair->value));
      pair = map_next(listas_repro);
      contador++;
  }
}

int main() {
  setlocale(LC_ALL, "es_ES.UTF-8"); // Para que se puedan ver tildes, ñ, y carácteres especiales.
  char opcion;
  Map *music_byid = map_create(is_equal_str);
  Map *music_bygenres = map_create(is_equal_str);
  Map *music_byartist = map_create(is_equal_str);
  Map *music_bytempo = map_create(is_equal_str);
  Map *listas_repro = map_create(is_equal_str);
  bool cargadas = false;
  do {
    mostrarMenuPrincipal();
    printf("Ingrese su opción: ");
    scanf(" %c", &opcion);
    printf("\n");

    switch (opcion) {
    case '1':
      if(cargadas) printf("¡Las canciones ya se cargaron!\n\n");
      else {
        cargadas = true;
        cargar_musica(music_byid, music_bygenres, music_byartist, music_bytempo);
      }  
      break;
    case '2':
      buscar_por_genero(music_bygenres);
      break;
    case '3':
      buscar_por_artista(music_byartist);
      break;
    case '4':
      buscar_por_tempo(music_bytempo);
      break;
    case '5':
      crear_lista_repro(listas_repro);
      break;
    case '6':
      agregar_cancion(listas_repro);
      break;
    case '7':
      break;
    case '8':
      puts("Saliendo del programa...");
      break;
    default:
      puts("Opción no válida. Por favor, intente de nuevo.");
    }
    if(opcion == '8') break;
    presioneTeclaParaContinuar();

  } while (opcion != '8');

  return 0;
}