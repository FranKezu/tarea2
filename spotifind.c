#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <locale.h>

typedef struct {
  char id[100];
  List *artists;
  char album_name[200];
  char track_name[200];
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

void barra_carga(long actual, long total) {

  int barra_largo = 40;
  float progreso = (float)actual / total;
  int llenos = (int)(progreso * barra_largo);

  printf("\rCargando canciones: [");
  for (int i = 0; i < barra_largo; i++) {
      if (i < llenos)
          wprintf(L"█"); // cuadrado lleno
      else
          wprintf(L"░"); // cuadrado vacío
  }
  printf("] %3d%% (%ld / %ld)", (int)(progreso * 100), actual, total);
  fflush(stdout);
}

int is_equal_str(void *key1, void *key2) {
  return strcmp((char *)key1, (char *)key2) == 0;
}

bool comprobacion(HashMap *map){
  if(firstMap(map) == NULL){
    printf("¡El programa no puede funcionar si no se han cargado canciones!\n\n");
    return false;
  }
  return true;
}

char *velocidad(float tempo){
  if(tempo < 80 ) return "Lentas";
  if (tempo <= 120) return "Moderadas";
  return "Rapidas";
}

void cargar_musica(HashMap  *music_byid, HashMap  *music_bygenres, HashMap  *music_byartist, HashMap  *music_bytempo) {
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
    
    strncpy(song->album_name, campos[3], 199);
    song->album_name[199] = '\0';
    
    strncpy(song->track_name, campos[4], 199);
    song->track_name[199] = '\0';
    
    song->tempo = atof(campos[18]);
    
    strncpy(song->track_genre, campos[20], 99);
    song->track_genre[99] = '\0';
    
    char *id_copy = strdup(song->id);
    insertMap(music_byid, id_copy, song);

    //HashMapa por género
    Pair *genre_pair = searchMap(music_bygenres, song->track_genre);
    if (genre_pair == NULL) {
      
      List *new_list = list_create();
      list_pushBack(new_list, song);
      char *genre_copy = strdup(song->track_genre);
      insertMap(music_bygenres, genre_copy, new_list);
    } else {
      List *genre_list = (List *)genre_pair->value;
      list_pushBack(genre_list, song);
    }

    //HashMap por artista
    char *artista = (char *)list_first(song->artists);
    while (artista != NULL) {
      Pair *artist_pair = searchMap(music_byartist, artista);
      if (artist_pair == NULL) {
        List *aux_list = list_create();
        list_pushBack(aux_list, song);
        char *artista_copy = strdup(artista);
        insertMap(music_byartist, artista_copy, aux_list);
      } else {
        List *artist_list = (List *)artist_pair->value;
        list_pushBack(artist_list, song);
      }
      artista = (char *)list_next(song->artists);
    }


    //HashMap por tempo
    char *tempo_categoria = velocidad(song->tempo);
    Pair *tempo_pair = searchMap(music_bytempo, tempo_categoria);
    if (tempo_pair == NULL) {
      
      List *new_list = list_create();
      list_pushBack(new_list, song);
      char *tempo_copy = strdup(tempo_categoria);
      insertMap(music_bytempo, tempo_copy, new_list);
    } else {
      List *tempo_list = (List *)tempo_pair->value;
      list_pushBack(tempo_list, song);
    }

    contador++;
    if (contador % 1000 == 0 || contador == 114000) {
      barra_carga(contador, 114000);
    }
  }
  fclose(archivo);
  printf("\nCanciones cargadas exitosamente!\n\n");
}

void mostrar_canciones(Pair *pair) {
  List *canciones = (List *)pair->value;

  if((Song *)list_first(canciones) == NULL){
    printf("La lista de reproducción no contiene canciones.\n");
    return;
  }

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

void buscar_por_genero(HashMap *music_bygenres) {
  if(!comprobacion(music_bygenres)) return;

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

  Pair *pair = searchMap(music_bygenres, genero);

  while(pair == NULL){
    printf("\nEl género ingresado no es válido. ¡Por favor ingrese uno de los siguientes géneros!\n\n");

    puts("╔═════════════════════════════════════════════════════════════╗");
    puts("║              Spotifind - Géneros disponibles                ║");
    puts("╠═════════════════════════════════════════════════════════════╣");
    
    for (int i = 0; i < 114; i++) {
        // Imprime 3 géneros por fila
        if (i % 3 == 0) printf("║");
    
        printf(" %-19s", lista_generos[i]);
    
        if ((i + 1) % 3 == 0)
            printf(" ║\n");
    }
    
    int restantes = 114 % 3;
    if (restantes != 0) {
        // Cierra la última fila si no es múltiplo de 3
        for (int j = 0; j < 3 - restantes; j++)
            printf(" %-20s", " ");
        printf(" ║\n");
    }
    puts("╚═════════════════════════════════════════════════════════════╝");
    printf("\n\n");
    printf("Ingrese el género de la canción: ");
    scanf("%s", genero);
    pair = searchMap(music_bygenres, genero);
  }

  mostrar_canciones(pair);
}

void buscar_por_artista(HashMap *music_byartist) {
  if(!comprobacion(music_byartist)) return;

  char artista[100];
  
  printf("\n─────────────────────────────────────────────────────────────────────────\n");
  printf("BÚSQUEDA POR ARTISTA\n");
  printf("─────────────────────────────────────────────────────────────────────────\n");
  printf("Ingrese el artista que desea buscar: ");
  scanf(" %[^\n]s", artista);

  Pair *pair = searchMap(music_byartist, artista);

  while(pair == NULL){
    printf("\nEl artista ingresado no se encontró.\n\n");
    printf("Ingrese el artista que desea buscar: ");
    scanf(" %[^\n]s", artista);
    pair = searchMap(music_byartist, artista);
  }
  
  mostrar_canciones(pair);
}

void buscar_por_tempo(HashMap *music_bytempo) {
  if(!comprobacion(music_bytempo)) return;

  char opcion;
  puts("╔════════════════════════════════════════════╗");
  puts("║            Spotifind - Elegir tempo        ║");
  puts("╠════════════════════════════════════════════╣");
  puts("║  1) Canciones lentas                       ║");
  puts("║  2) Canciones moderadas                    ║");
  puts("║  3) Canciones rápidas                      ║");
  puts("╚════════════════════════════════════════════╝");
  printf("Ingrese el tempo que desea buscar: ");
  scanf(" %c", &opcion);

  while(opcion != '1' && opcion != '2' && opcion != '3'){
    puts("Opción no válida. Por favor, intente de nuevo.");
    printf("Ingrese el tempo que desea buscar: ");
    scanf(" %c", &opcion);
  }

  char *tempo;
  if(opcion == '1') tempo = strdup("Lentas");
  else if(opcion == '2') tempo = strdup("Moderadas");
  else tempo = strdup("Rapidas");
  
  Pair *pair = searchMap(music_bytempo, tempo);

  while(pair == NULL){
    printf("\nLa categoria de velocidad que busca no existe .\n\n");
    printf("Ingrese la velocidad que desea buscar (Lentas-Moderadas-Rápidas): ");
    scanf(" %[^\n]s", tempo);
    pair = searchMap(music_bytempo, tempo);
  }

  free(tempo);
  mostrar_canciones(pair);
}

void crear_lista_repro(HashMap *listas_repro){
  char nombre[100];
  printf("Ingrese el nombre para su lista de reproducción: ");
  scanf(" %[^\n]s", nombre);
  Pair *pair = searchMap(listas_repro, nombre);

  if (pair == NULL) {
    List *new_list = list_create();
    char *nombre_copia = strdup(nombre);
    insertMap(listas_repro, nombre_copia, new_list);
    printf("\nLista de reproducción (%s) creada con éxito.\n", nombre_copia);
  } else {
    printf("\nYa existe una lista de reproducción con ese nombre, por favor intenta con otro.\n");
  }

}

bool mostrar_listas_creadas(HashMap *repro){

  Pair *pair = firstMap(repro);
  if (pair == NULL) {
    printf("No se encontraron listas de reproducción para mostrar.\n");
    printf("Por favor, crea una lista primero.\n\n");
    return false;
  }
  puts("╔═══════════════════════════════════════════════════╗");
  puts("║           Listas de reproducción creadas          ║");
  puts("╠═════╦═════════════════════════════╦═══════════════╣");
  puts("║  N° ║ Nombre                      ║ Canciones     ║");
  puts("╠═════╬═════════════════════════════╬═══════════════╣");

  int contador = 1;

  while (pair != NULL) {
    char *nombre = (char *)pair->key;
    List *lista = (List *)pair->value;
    printf("║ %2d  ║ %-28s║     %4d      ║\n", contador, nombre, list_size(lista));
    contador++;
    pair = nextMap(repro);
  }

  puts("╚═════╩═════════════════════════════╩═══════════════╝");
  return true;
}


void agregar_cancion(HashMap *listas_repro, HashMap *music_byid) {
  if(!comprobacion(music_byid)) return;
  bool flag = mostrar_listas_creadas(listas_repro);
  if(!flag) return;
  
  char nombre_lista[100];
  printf("Ingrese el nombre de la lista que desea agregar una cancion:");
  scanf(" %[^\n]s", nombre_lista);

  Pair *lista_pair = searchMap(listas_repro, nombre_lista);
  while(lista_pair == NULL){
    printf("El nombre ingresado de la lista no existe\n");
    printf("Por favor ingresa uno valido:");
    
    scanf(" %[^\n]s", nombre_lista);
    lista_pair = searchMap(listas_repro, nombre_lista);
  }

  char ID[100];
  printf("Ingrese el ID de la cancion que desea ingresar:");
  scanf(" %[^\n]s", ID);

  Pair *song_pair = searchMap(music_byid, ID);
  while(song_pair == NULL){
    printf("El ID ingresado no ha encontrado\n");
    printf("Ingrese un ID válido:");
    scanf(" %[^\n]s", ID);
    song_pair = searchMap(music_byid, ID);
  }

  Song *cancion = (Song *)song_pair->value;
  List *lista = (List *)lista_pair->value;
  list_pushBack(lista, cancion);

  printf("\nCanción \"%s\" del artista agregada exitosamente a la lista \"%s\".\n", cancion->track_name, nombre_lista);
}

void mostrar_lista_repro(HashMap *listas_repro){

  bool flag = mostrar_listas_creadas(listas_repro);
  if(!flag) return;

  char nombre_lista[100];
  printf("\nIngrese el nombre de la lista que desea ver: ");
  scanf(" %[^\n]s", nombre_lista);

  Pair *lista_pair = searchMap(listas_repro, nombre_lista);
  while (lista_pair == NULL) {
    printf("El nombre ingresado no corresponde a ninguna lista.\n");
    printf("Por favor, ingrese un nombre válido: ");
    scanf(" %[^\n]s", nombre_lista);
    lista_pair = searchMap(listas_repro, nombre_lista);
  }

  mostrar_canciones(lista_pair);
}

int main() {
  setlocale(LC_ALL, "es_ES.UTF-8"); // Para que se puedan ver tildes, ñ, y carácteres especiales.
  char opcion;
  HashMap *music_byid = createMap(200000);       // Capacidad primo cercana
  HashMap *music_bygenres = createMap(1000);      // Mucho menos géneros (~114 max)
  HashMap *music_byartist = createMap(50000);    // Si hay miles de artistas
  HashMap *music_bytempo = createMap(10);         // Solo 3: lenta, moderada, rápida
  HashMap *listas_repro = createMap(100);  
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
      agregar_cancion(listas_repro, music_byid);
      break;
    case '7':
      mostrar_lista_repro(listas_repro);
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