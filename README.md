# Task Management

Aplicación de escritorio construida con Qt 6, Qt Widgets, C++17, CMake y
SQLite. Cada usuario tiene su propia sesión y solamente puede consultar o
modificar sus tareas.

## Funcionalidades

- Registro e inicio de sesión local.
- Contraseñas protegidas con PBKDF2-SHA256 y sal aleatoria.
- Creación, edición, completado y eliminación de tareas.
- Prioridades baja, normal y alta.
- Estados pendiente, en progreso y completada.
- Búsqueda y filtros por estado y prioridad.
- Fechas límite y detección visual de tareas vencidas.
- Recordatorios mediante notificaciones del sistema.
- Temas claro y oscuro con preferencia persistente.
- Menú lateral plegable y navegación por teclado en autenticación.
- Base de datos SQLite creada automáticamente.
- Prueba integrada del flujo de autenticación y tareas.

## Abrir en Qt Creator

1. Abre `CMakeLists.txt` desde Qt Creator.
2. Selecciona el Kit **Desktop Qt 6.11.1 MinGW 64-bit**.
3. Ejecuta **Build > Run CMake**.
4. Ejecuta **Build > Build Project**.
5. Inicia la aplicación con `Ctrl+R`.

La base de datos también puede inicializarse sin abrir la interfaz ejecutando
el programa con el argumento `--initialize-database`.

## Organización

```text
src/
├── database/   Conexión SQLite y repositorios
├── models/     User y Task
├── security/   Derivación y verificación de contraseñas
├── services/   Autenticación y recordatorios
└── ui/         Widgets y formularios de Qt Designer

resources/
└── styles/     Temas claro y oscuro con acento rojo

tests/
└── TaskManagementTests.cpp
```

Qt Creator muestra los `.h` dentro del grupo virtual **Header Files**, los
`.cpp` dentro de **Source Files** y los `.ui` dentro de **Forms**, aunque
físicamente estén organizados en las carpetas anteriores.

## Base de datos

En Windows, SQLite se guarda en `%APPDATA%\TaskManagement`. En otros sistemas
se utiliza la carpeta devuelta por `QStandardPaths::AppDataLocation`. El archivo
se llama:

```text
task-management.db
```

La variable opcional `TASK_MANAGEMENT_DATA_DIR` permite utilizar otra carpeta;
las pruebas la usan para trabajar con una base de datos temporal independiente.

## Pruebas

Desde Qt Creator puedes seleccionar el objetivo `TaskManagementTests`. Desde
una terminal configurada con el Kit también puedes ejecutar CTest dentro del
directorio de compilación.

La prueba integrada verifica:

1. Registro y cierre de sesión.
2. Inicio de sesión con la cuenta creada.
3. Creación y filtrado de una tarea.
4. Edición y cambio de estado.
5. Marcado como completada.
6. Eliminación.

## Evolución futura

SQLite es apropiado para esta versión local. Para compartir tareas entre varias
computadoras se recomienda añadir una API y una base centralizada como SQL
Server o PostgreSQL, manteniendo las pantallas y modelos actuales.
