# 🎧 Tarea 2: Sistema de Listas de Reproducción - **Spotifind**

* **👨‍💻 Integrantes:** Ariel Leiva y Franco Bernal
* **📚 Asignatura:** Estructura de Datos (ICI2240-2)
* **📅 Fecha de entrega:** 11-05-2025

---

## 🚀 ¿Cómo ejecutar el programa?

### 🟢 Opción 1: Ejecutar el archivo `.exe`

Puedes ejecutar el programa directamente haciendo doble clic en el archivo `tarea2.exe` que se encuentra en la carpeta del proyecto.
Esto abrirá automáticamente una consola con el programa corriendo.

También puedes hacerlo desde Visual Studio Code ejecutando:

```bash
./run
```

---

### 🛠️ Opción 2: Compilar manualmente

1. Abre la consola de Visual Studio Code como por ejemplo.
2. Compila el programa con el siguiente comando:

```bash
gcc tarea2.c tdas/list.c tdas/map.c tdas/extra.c -o tarea2
```

3. Luego ejecútalo con:

```bash
./tarea2
```

En caso de ver carácteres raros, es porque no se están mostrando los tildes correctamente, para solucionar esto basta con escribir lo siguiente la primera vez antes de ejecturar el programa.

```powershell
$OutputEncoding = [Console]::OutputEncoding = [Text.UTF8Encoding]::new()
```


---

### 📋 ¿Qué hace el programa?

Este programa, llamado Spotifind, es un sistema de gestión de canciones que permite cargar datos desde un archivo CSV y organizarlos eficientemente utilizando estructuras como HashMap y List. Ofrece funciones para buscar canciones por género, artista o velocidad (tempo), así como crear listas de reproducción personalizadas y agregarles canciones. Es ideal para manejar colecciones musicales de forma dinámica y estructurada, facilitando la búsqueda y clasificación de canciones según distintos criterios.

---

### 📂 Archivos requeridos (Incluídos en el repositorio)

- `tarea2.c`: contiene el código del programa.
- `tdas/list.c` y `tdas/list.h`:
- `tdas/extra.c` y `tdas/extra.h`

---

### 🔧 Requisitos previos (sólo si deseas compilarlo manualmente)

- Tener instalado **Visual Studio Code.**
- Instalar la extensión para **C/C++**.
- Tener instalado un compilador para C, como **MinGW.**

---

### 📋 Explicación del menú del programa:

## 1️⃣ FUNCION  (📂 Cargar canciones desde archivo CSV)

- Lee los datos del archivo song_dataset.csv.

- Llena las estructuras (HashMaps, listas) con la información de las canciones.

- Muestra una barra de carga animada mientras se procesan los datos.

## 2️⃣ FUNCION (🔍 Buscar canciones por género)

- Pide al usuario que ingrese un género (por ejemplo: Pop, Rock, Jazz).

- Muestra todas las canciones de ese género.

## 3️⃣ FUNCION  (🎤 Buscar canciones por artista)

- Solicita el nombre de un artista.

- Muestra todas las canciones que tiene ese artista en la base de datos.

## 4️⃣ FUNCION  (⏱️ Buscar canciones por velocidad (tempo))

Permite buscar canciones por tempo:

   + Lentas (tempo < 90)

   + Moderadas (90 ≤ tempo ≤ 120)

   + Rápidas (tempo > 120)

## 5️⃣ FUNCION  (🎵 Crear lista de reproducción)

- El usuario asigna un nombre a una nueva lista.

- La lista queda disponible para agregar canciones.

## 6️⃣ FUNCION (➕ Agregar canción a una lista)

- Pide el nombre de una canción y el nombre de la lista.

- Si existen ambos, la canción se añade a la lista correspondiente.

## 7️⃣ FUNCION (📄 Mostrar canciones de una lista)

- Pide el nombre de una lista de reproducción.

- Muestra todas las canciones que contiene esa lista.

## 8️⃣ FUNCION (🚪 Salir del programa)

- Termina la ejecución del programa de forma segura.
  
**Antes de salir del programa, puedes ejecutar todas las opciones del menú tantas veces lo necesites.**

### 🚀 Ejemplo simple de uso

**Iniciar el programa**:
   ```bash
   🎵 Bienvenido al gestor de canciones 🎵

   1) Cargar canciones 
   2) Buscar por género
   3) Buscar por artista 
   4) Buscar por tempo  
   5) Crear lista de reproducción  
   6) Agregar canción a la lista   
   7) Mostrar canciones de la lista  
   8) Salir
```
1. **Cargar Canciones**:
   - Selecciona la opción `1`.
   ```
   "Las canciones se han cargado correctamente."
   ```

2. **Buscar por género**:
   - Selecciona la opción `2`.
   ```
    Ingrese el género de la canción: Anime
    "Se muestran todas las canciones con ese género"
    ```
3. **Buscar por artista**:
   - Selecciona la opción `3`.
   ```
   Ingrese el artista que desea buscar: Michael Jackson
   "Se muestran todas las canciones en las que participa el artista"
   ```
4. **Buscar por tempo**:
   - Selecciona la opción `4`.
   ```
   1) Canciones lentas
   2) Canciones moderadas
   3) Canciones rápidas
   Ingrese la opción del tempo que desea buscar: 2
   "Se muestran todas las canciones que tengan el tempo 'Moderado'. "
   ```
5. **Crear lista de reproducción**:
   - Selecciona la opción `5`.
   ```
    Ingrese el nombre para su lista de reproducción: Favoritas
    "Lista de reproducción (Favoritas) creada con éxito."
   ```
6. **Agregar canción a la lista**:
    - Selecciona la opción `6`.
   ```
   Ingrese el nombre de la lista que desea agregar una cancion: Favoritas.
   Ingrese el ID de la cancion que desea ingresar: 3125
   "Canción "..." agregada exitosamente a la lista Favoritas."
   ``` 
7. **Mostrar canciones de la lista **:
    - Selecciona la opción `7`.
   ```
   Ingrese el nombre de la lista que desea ver: Favoritas
   "Lista de canciones:"
   ...
   ``` 
---

### 🐞 Posibles problemas encontrados

- **Soporte**: Solo Windows, usuarios de MacOS podrían tener dificultades ejecutando el programa.
- **Entrada inválida**: si se ingresan caracteres cuando se solicita un número, el programa puede trabarse.
- **Buscar por Tempo**: Puede ser algo lento para el usuario, esto sucede por la cantidad de prints que debe realizar el programa (debe recorrer 114000 canciones e imprimir las que esten en el rango del tempo que desea el usuario). 
