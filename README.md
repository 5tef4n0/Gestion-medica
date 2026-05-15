🏥 Sistema de Gestión Médica y Clínico🎯 ObjetivoDesarrollar un sistema de consola avanzado en lenguaje C estructurado y modular para la gestión eficiente de pacientes, médicos, agendamiento de citas y fichas clínicas electrónicas (EMR). El sistema incluye autenticación por roles, validación estricta de signos vitales (Triage), algoritmo anti-colisiones y persistencia segura de datos mediante archivos de texto (CSV).👥 IntegrantesStefano CabezasZander Castillo✨ Características Principales🔒 Seguridad y Sincronización Automática: Sistema de login por roles (Admin, Médico, Paciente). Incluye un "sincronizador en caliente" que detecta nuevos registros en la base de datos y les genera credenciales automáticamente.🩺 Historial Clínico y Triage: Los médicos pueden atender a los pacientes llenando un formulario clínico completo validado (Peso en kg, Frecuencia cardíaca, Presión arterial sistólica/diastólica, síntomas y diagnóstico).🛡️ Prevención de Errores (Sanitización): Algoritmos de limpieza de memoria y sanitización de texto para evitar que el usuario corrompa la base de datos CSV al ingresar caracteres reservados como el punto y coma (;).🧠 Algoritmo Anti-Colisiones: Detección de disponibilidad en tiempo real para evitar el doble agendamiento en la misma franja horaria.💾 Persistencia de Datos a Gran Escala: Capacidad demostrada para manejar bases de datos con +200 pacientes, +100 médicos y +360 citas simultáneas usando estructuras en memoria RAM y volcado a .txt.🏗️ Arquitectura del ProyectoEl sistema está diseñado priorizando un código limpio y de alta cohesión, dividiendo responsabilidades en múltiples librerías:/
├── main.c           # Orquestador principal y bucles de menú
├── config.h         # Límites de memoria y rutas de base de datos
├── auth.h/.c        # Módulo de Autenticación y Sincronizador Mágico
├── pacientes.h/.c   # Módulo CRUD de Pacientes (incluye emails)
├── medicos.h/.c     # Módulo CRUD de Médicos y horarios
├── citas.h/.c       # CORE: Agendamiento, Triage y Visor de Fichas Detalladas
├── reportes.h/.c    # Generación de Estadísticas
├── utils.h/.c       # Utilidades UI (Colores ANSI, Validaciones numéricas)
└── data/            # Bases de datos (.txt)
    ├── pacientes.txt
    ├── medicos.txt
    ├── citas.txt
    └── usuarios.txt
🚀 Instrucciones para compilarPara compilar y ejecutar el proyecto desde la terminal, asegúrate de tener GCC instalado (MinGW/MSYS2 en Windows) y utiliza los siguientes comandos en la carpeta raíz del proyecto:# 1. Compilar todos los archivos fuente juntos
gcc *.c -o gestion

# 2. Ejecutar el sistema (Windows)
.\gestion.exe

# 3. Ejecutar el sistema (Linux / macOS)
./gestion
⚠️ Importante: El sistema requiere que exista una carpeta llamada data/ en el mismo directorio donde se ejecuta. Si compilas el código desde cero, asegúrate de crear esta carpeta para que las bases de datos puedan guardarse correctamente.📸 Evidencia
