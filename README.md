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

NO SE PO AVERIGUA

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

#### 1️⃣ FUNCION  
- Falta texto ariel :v

#### 2️⃣ FUNCION
- Falta texto ariel :v

#### 3️⃣ FUNCION  
- Falta texto ariel :v

#### 4️⃣ FUNCION  
- Falta texto ariel :v

#### 5️⃣ FUNCION  
- Falta texto ariel :v

#### 6️⃣ FUNCION
- Falta texto ariel :v


**Antes de salir del programa, puedes ejecutar todas las opciones del menú tantas veces lo necesites.**

---

### 🚀 Ejemplo simple de uso

1. **Iniciar el programa**:
   - WAAAAAAAAAAAAAAAA EJEMPLO

2. **Registrar un ticket**:
   - Selecciona la opción `1`.
   - Ingresa un ID, por ejemplo: `19`.
   - Describe el problema: `PC no enciende.`.
   - El ticket se registra con prioridad baja por defecto.

3. **Cambiar prioridad**:
   - Selecciona la opción `2`.
   - Ingresa el ID: `19`.
   - Elige nueva prioridad: `1` (Alta).
   - El ticket se reordena en la lista según su nueva prioridad.

4. **Mostrar lista de tickets**:
   - Selecciona la opción `3`.
   - Verás una lista de todos los tickets ingresados, incluyendo el que acabas de ingresar ID `19`. Estarán ordenados por hora y prioridad.
5. **Procesar ticket**:
   - Selecciona la opción `4`.
   - El ticket ID `19` se muestra junto a sus datos como el siguiente a procesar y se elimina de la lista.

6. **Salir**:
   - Selecciona la opción `6` para finalizar el programa.

---

### 🐞 Posibles problemas encontrados

- **Rendimiento lento**: la carga de canciones puede ser algo lenta, provocando impaciencia al usuario.
- **Soporte**: Solo Windows, usuarios de MacOS podrían tener dificultades ejecutando el programa.
- **Entrada inválida**: si se ingresan caracteres cuando se solicita un número, el programa puede trabarse.