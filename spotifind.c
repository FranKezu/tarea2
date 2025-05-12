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

//Muestra una barra de progreso en la consola para indicar visualmente el avance de carga de las canciones.
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

//Verifica si el mapa hash (HashMap) que contiene las canciones ha sido cargado correctamente antes de continuar con el programa. 
bool comprobacion(HashMap *map){
  if(firstMap(map) == NULL){
    printf("¡El programa no puede funcionar si no se han cargado canciones!\n\n");
    return false;
  }
  return true;
}

//Clasifica una canción según su tempo (velocidad musical) y devuelve una cadena de texto que describe dicha categoría.
char *velocidad(float tempo){
  if(tempo < 80 ) return "Lentas";
  if (tempo <= 120) return "Moderadas";
  return "Rapidas";
}

/*Se encarga de leer un archivo CSV llamado "song_dataset.csv" y cargar la información de cada canción en varias estructuras HashMap 
que organizan los datos según diferentes criterios: por ID, género, artista y tempo.*/
void cargar_musica(HashMap  *music_byid, HashMap  *music_bygenres, HashMap  *music_byartist, HashMap  *music_bytempo) {
  FILE *archivo = fopen("song_dataset.csv", "r");
  if (archivo == NULL) {
    perror(
        "Error al abrir el archivo"); 
    return;
  }
  
  char **campos;
  campos = leer_linea_csv(archivo, ','); // => Leemos la linea entera, o sea, todos los datos de la canción.

  long contador = 0;
  while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
    // De aquí para abajo utilizamos solo los datos que necesitamos, copiando a una estructura y asignando cada uno a los hashMaps correspondientes.
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
    
    //HashMap por IDs
    char *id_copy = strdup(song->id); // => Se crea una copia de la ID para guardar en el mapa.
    insertMap(music_byid, id_copy, song);

    //HashMap por género
    Pair *genre_pair = searchMap(music_bygenres, song->track_genre);
    if (genre_pair == NULL) { // => En caso de que no haya una lista con ese género en el mapa, sea crea una nueva.
      List *new_list = list_create();
      list_pushBack(new_list, song);
      char *genre_copy = strdup(song->track_genre);
      insertMap(music_bygenres, genre_copy, new_list);
    } else { // => Si ya está creada solo ingresa el pair.
      List *genre_list = (List *)genre_pair->value;
      list_pushBack(genre_list, song);
    }

    //HashMap por artista
    char *artista = (char *)list_first(song->artists);
    while (artista != NULL) { //=> Se finaliza hasta que no se encuentren más artistas (== NULL).
      Pair *artist_pair = searchMap(music_byartist, artista);
      if (artist_pair == NULL) { // En caso de que la lista del artista no exista, se crea una con su respectivo nombre como clave
        List *aux_list = list_create();
        list_pushBack(aux_list, song);
        char *artista_copy = strdup(artista); // => Creamos una copia del artista, para evitar modificaciones imprevistas en el mapa.
        insertMap(music_byartist, artista_copy, aux_list);
      } else { //=> Se asume que la lista ya está creada, por lo que se agrega la canción a la lista de su respectivo Artista(s).
        List *artist_list = (List *)artist_pair->value;
        list_pushBack(artist_list, song);
      }
      artista = (char *)list_next(song->artists); // => Pasamos al siguiente artista.
    }

    //HashMap por tempo => Se repiten los mismos métodos explicados en las funciones de arriba.
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
    if (contador % 1000 == 0 || contador == 114000) { // => Actualiza barra de carga cada 1000 canciones.
      barra_carga(contador, 114000);
    }
  }
  fclose(archivo);
  printf("\nCanciones cargadas exitosamente!\n\n");
}


/*Imprime en pantalla la información de una lista de canciones contenida en un Pair. 
Si la lista está vacía, muestra un mensaje de advertencia. En caso contrario, 
recorre cada canción y muestra su ID, artistas (o "Desconocido" si no hay)*/
void mostrar_canciones(Pair *pair, long total) {
  List *canciones = (List *)pair->value;

  if((Song *)list_first(canciones) == NULL){
    printf("La lista de reproducción no contiene canciones.\n");
    return;
  }

  printf("\nLista de canciones:\n");
  Song *cancion = (Song *)list_first(canciones);

  long i = 0;
  while (cancion != NULL && (total == -1 || i < total)) { // => Se muestran todas las canciones, o un valor específico según se requiera.
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
    i++;
    printf(" | Álbum: %s | Canción: %s | Tempo: %.0f | Género: %s\n",
    cancion->album_name, cancion->track_name, cancion->tempo, cancion->track_genre);
    cancion = (Song *)list_next(canciones);
  }
}

/*Permite al usuario buscar canciones por género musical dentro del programa. 
Primero, verifica que haya canciones cargadas usando la función comprobacion. 
Luego, solicita al usuario que ingrese un género. Si el género no existe en el HashMap de géneros, 
se muestra una lista completa de los 114 géneros disponibles y se solicita nuevamente la entrada hasta que sea válida.*/
void buscar_por_genero(HashMap *music_bygenres) {
  if(!comprobacion(music_bygenres)) return;

  char *lista_generos[114] = { // => Lista con todos los géneros para mostrarlos al usuario.
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
  
  puts("╔═══════════════════════════════════════════════════════════════════════════╗");
  puts("║                Spotifind - Búsqueda por género musical                    ║");
  puts("╚═══════════════════════════════════════════════════════════════════════════╝");
  printf("Ingrese el género de la canción: ");
  scanf("%s", genero);

  Pair *pair = searchMap(music_bygenres, genero);

  while(pair == NULL){
    printf("\nEl género ingresado no es válido. ¡Por favor ingrese uno de los siguientes géneros!\n\n");
    // Como el usuario ingresó un genero que no existe, se le muestran de manera bonita y clara.
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

  mostrar_canciones(pair, -1); // => El -1 le indica a la función mostrar todas las canciones que se puedan.
}

/*Permite al usuario buscar canciones asociadas a un artista específico. 
Primero, se asegura de que haya canciones cargadas usando la función comprobacion. 
Luego, solicita al usuario que ingrese el nombre del artista.*/
void buscar_por_artista(HashMap *music_byartist) {
  if(!comprobacion(music_byartist)) return;

  char artista[100];
  
puts("╔═══════════════════════════════════════════════════════════════════════════╗");
puts("║                   Spotifind - Búsqueda por artista                        ║");
puts("╚═══════════════════════════════════════════════════════════════════════════╝");
  printf("Ingrese el artista que desea buscar: ");
  scanf(" %[^\n]s", artista);

  Pair *pair = searchMap(music_byartist, artista);

  while(pair == NULL){
    printf("\nEl artista ingresado no se encontró.\n\n");
    printf("Ingrese el artista que desea buscar: ");
    scanf(" %[^\n]s", artista);
    pair = searchMap(music_byartist, artista);
  }
  
  mostrar_canciones(pair, -1); // => -1 indica que se mostrarán todas las canciones.
}

/*Permite al usuario buscar canciones según la velocidad de su tempo: lentas, moderadas o rápidas. 
Tras comprobar que hay canciones cargadas, muestra un menú con tres opciones numeradas. 
El usuario debe seleccionar una opción válida; en caso contrario, se le vuelve a pedir la entrada. 
Según la opción elegida, se asigna la categoría correspondiente ("Lentas", "Moderadas" o "Rapidas")*/
void buscar_por_tempo(HashMap *music_bytempo) {
  if(!comprobacion(music_bytempo)) return;

  char opcion;
  puts("╔════════════════════════════════════════════╗");
  puts("║         Spotifind - Búsqueda por tempo     ║");
  puts("╠════════════════════════════════════════════╣");
  puts("║  1) Canciones lentas                       ║");
  puts("║  2) Canciones moderadas                    ║");
  puts("║  3) Canciones rápidas                      ║");
  puts("╚════════════════════════════════════════════╝");
  printf("Ingrese la opción del tempo que desea buscar: ");
  scanf(" %c", &opcion);

  //Validamos que la entrada sea correcta, caso contrario se pregunta nuevamente. (Misma forma se utiliza en otras funciones)
  while(opcion != '1' && opcion != '2' && opcion != '3'){
    puts("Opción no válida. Por favor, intente de nuevo.");
    printf("Ingrese el tempo que desea buscar: ");
    scanf(" %c", &opcion);
  }

  char *tempo;
  //La opción la asignamos a la categoria correspondiente.
  if(opcion == '1') tempo = strdup("Lentas");
  else if(opcion == '2') tempo = strdup("Moderadas");
  else tempo = strdup("Rapidas");
  
  Pair *pair = searchMap(music_bytempo, tempo);

  //De aquí para abajo se pregunta al usuario se desea mostrar todas las canciones o un número personalizado.
  //Además se valida que la entrada sea correcta, en caso contrario se vuelve a preguntar.

  long cant;
  puts("╔════════════════════════════════════════════╗");
  puts("║      Spotifind - Seleccionar cantidad      ║");
  puts("╠════════════════════════════════════════════╣");
  puts("║  1) Todas las canciones                    ║");
  puts("║  2) Cantidad personalizada                 ║");
  puts("╚════════════════════════════════════════════╝");

  printf("Ingrese una opción: ");
  scanf(" %c", &opcion);
  while(opcion != '1' && opcion != '2'){
    puts("Opción no válida. Por favor, intente de nuevo.");
    printf("Ingrese la opción que desea: ");
    scanf(" %c", &opcion);
  }
  if(opcion == '1') cant = -1; // => -1 le indica a la función mostrar todas las canciones.
  else {
    puts("¿Cuántas canciones desea mostrar?: ");
    scanf(" %ld", &cant);
    while(cant < 0){
      puts("No puedes ingresar un número negativo. Intente nuevamente.");
      printf("\n¿Cuántas canciones desea mostrar?: ");
      scanf(" %ld", &cant);
    }
  }
  
  free(tempo);
  mostrar_canciones(pair, cant);
}

/*Usamos esta función para crear una lista con el nombre que el usuario indique, para luego almacenar canciones dentro de esta.
La lista se almacena en un HashMap, donde la key es el nombre de la lista, y el value la propia lista.*/
void crear_lista_repro(HashMap *listas_repro){
  char nombre[100];
  printf("Ingrese el nombre para su lista de reproducción: ");
  scanf(" %[^\n]s", nombre);
  Pair *pair = searchMap(listas_repro, nombre);

  if (pair == NULL) { // => Significa que la lista no está creada, por lo que se crea una con el nombre ingresado por el ususario y se ingresa en el HashMap.
    List *new_list = list_create();
    char *nombre_copia = strdup(nombre);
    insertMap(listas_repro, nombre_copia, new_list);
    printf("\nLista de reproducción (%s) creada con éxito.\n", nombre_copia);
  } else {
    printf("\nYa existe una lista de reproducción con ese nombre, por favor intenta con otro.\n"); //=> Si ya existe se le informa al usuario.
  }
}

/*Muestra en pantalla todas las listas de reproducción que el usuario ha creado. 
Primero verifica si hay listas disponibles en el HashMap; si no las hay, informa al usuario y retorna false. 
Si existen, imprime una tabla con el número, nombre y cantidad de canciones de cada lista.*/
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

/*Permite al usuario añadir una canción a una lista de reproducción existente. 
Primero comprueba que haya canciones cargadas y listas creadas; si no es así, la función termina. 
Luego solicita al usuario el nombre de la lista y lo valida hasta que se encuentre en el HashMap. 
A continuación, pide el ID de una canción, también validándolo.
Una vez encontrados la lista y la canción, esta se agrega al final de la lista*/
void agregar_cancion(HashMap *listas_repro, HashMap *music_byid) {
  // Las 3 primeras lineas comprueban que todo esté bien para el funcionamiento de la función.
  if(!comprobacion(music_byid)) return;
  bool flag = mostrar_listas_creadas(listas_repro);
  if(!flag) return;
  
  char nombre_lista[100];
  printf("Ingrese el nombre de la lista que desea agregar una cancion:");
  scanf(" %[^\n]s", nombre_lista);

  Pair *lista_pair = searchMap(listas_repro, nombre_lista); // Se busca la lista ingresada.
  while(lista_pair == NULL){ // => Si no existe se pregunta nuevamente.
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
  //Sacamos los valores que están asociados para agregar a la lista indicada por el usuario.
  Song *cancion = (Song *)song_pair->value;
  List *lista = (List *)lista_pair->value;
  list_pushBack(lista, cancion);

  printf("\nCanción \"%s\" exitosamente a la lista \"%s\".\n", cancion->track_name, nombre_lista);
}

/*Permite al usuario ver las canciones de una lista de reproducción específica. 
Primero, solicita al usuario el nombre de la lista que desea visualizar.  
Una vez validada, llama a mostrar_canciones para mostrar las canciones de la lista seleccionada.*/
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

  mostrar_canciones(lista_pair, -1);
}

int main() {
  setlocale(LC_ALL, "es_ES.UTF-8"); // Para que se puedan ver tildes, ñ, y carácteres especiales.
  char opcion;
  //Creamos todos los hashmaps para utilizar, con capacidad de sobra. Aunque en un caso concreto de quedarse sin memoria este aumenta automáticamente.
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

  free(music_byid);
  free(music_byartist);
  free(music_byid);
  free(music_bytempo);
  free(listas_repro);
  
  return 0;
}
