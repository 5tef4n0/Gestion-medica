# 🏥 Sistema de Gestión Médica y Clínico

Sistema de consola avanzado desarrollado en lenguaje C estructurado y modular para la gestión eficiente de pacientes, médicos, citas médicas y fichas clínicas electrónicas (EMR).

---

# 🎯 Objetivo

Desarrollar un sistema robusto capaz de administrar:

- 👨‍⚕️ Médicos
- 🧑‍🤝‍🧑 Pacientes
- 📅 Citas médicas
- 📋 Historial clínico electrónico
- 🔐 Usuarios y autenticación por roles

El proyecto implementa persistencia mediante archivos `.txt`, estructuras en memoria RAM y validaciones avanzadas para evitar inconsistencias en los datos.

---

# 👥 Integrantes

- Stefano Cabezas
- Zander Castillo

---

# ✨ Características Principales

## 🔒 Seguridad y Sincronización Automática

- Sistema de autenticación por roles:
  - Administrador
  - Médico
  - Paciente

- Sincronizador automático de usuarios:
  - Detecta nuevos registros
  - Genera credenciales automáticamente
  - Actualiza la base de usuarios en caliente

---

## 🩺 Historial Clínico y Triage

Los médicos pueden registrar información clínica completa:

- Peso del paciente
- Frecuencia cardíaca
- Presión arterial
- Síntomas
- Diagnóstico
- Receta médica

Incluye validaciones estrictas de signos vitales.

---

## 🛡️ Prevención de Errores (Sanitización)

El sistema implementa algoritmos de sanitización de texto para:

- Evitar corrupción de archivos CSV/TXT
- Limpiar memoria
- Bloquear caracteres reservados como:

```txt
;
```

---

## 🧠 Algoritmo Anti-Colisiones

El sistema verifica disponibilidad en tiempo real para evitar:

- Doble agendamiento
- Choques de horarios
- Citas repetidas

---

## 💾 Persistencia de Datos a Gran Escala

Capacidad demostrada para manejar:

- ✅ +200 pacientes
- ✅ +100 médicos
- ✅ +360 citas médicas

Todo utilizando:

- Estructuras dinámicas en memoria RAM
- Persistencia mediante archivos `.txt`

---

# 🏗️ Arquitectura del Proyecto

```txt
/
├── main.c           # Orquestador principal y bucles de menú
├── config.h         # Límites de memoria y rutas de base de datos
├── auth.h/.c        # Módulo de autenticación y sincronizador
├── pacientes.h/.c   # CRUD de pacientes
├── medicos.h/.c     # CRUD de médicos y horarios
├── citas.h/.c       # Agendamiento, triage e historial clínico
├── reportes.h/.c    # Generación de estadísticas
├── utils.h/.c       # Utilidades y validaciones
└── data/
    ├── pacientes.txt
    ├── medicos.txt
    ├── citas.txt
    └── usuarios.txt
```

---

# 🚀 Tecnologías Utilizadas

- Lenguaje C
- GCC / MinGW
- Archivos TXT tipo CSV
- Consola ANSI
- Programación modular
- Estructuras y punteros

---

# ⚙️ Instrucciones de Compilación

## 📌 Requisitos

Instalar:

- GCC
- MinGW/MSYS2 (Windows)

---

## 🔨 Compilar el proyecto

```bash
gcc *.c -o gestion
```

---

## ▶️ Ejecutar en Windows

```bash
.\gestion.exe
```

---

## ▶️ Ejecutar en Linux/macOS

```bash
./gestion
```

---

# ⚠️ Importante

El sistema requiere una carpeta llamada:

```txt
data/
```

Esta carpeta debe existir en el mismo directorio donde se ejecuta el programa.

Si compilas el proyecto desde cero, asegúrate de crearla manualmente.

---

# 📂 Bases de Datos Utilizadas

| Archivo | Descripción |
|---|---|
| pacientes.txt | Información de pacientes |
| medicos.txt | Información de médicos |
| citas.txt | Registro de citas médicas |
| usuarios.txt | Credenciales y autenticación |

---

# 📊 Funcionalidades del Sistema

## 👨‍⚕️ Gestión de Médicos

- Registrar médicos
- Editar información
- Eliminar médicos
- Configurar horarios

---

## 🧑 Gestión de Pacientes

- Registrar pacientes
- Actualizar información
- Buscar pacientes
- Eliminar pacientes

---

## 📅 Gestión de Citas

- Agendar citas
- Cancelar citas
- Reagendar
- Validar disponibilidad

---

## 📋 Historial Clínico

- Registrar consultas
- Registrar signos vitales
- Diagnósticos
- Recetas médicas

---

## 📈 Reportes

- Estadísticas generales
- Cantidad de citas
- Pacientes atendidos
- Reportes médicos

---

# 🧪 Validaciones Implementadas

- Validación de correos electrónicos
- Validación de cédula
- Validación de teléfonos
- Validación de presión arterial
- Validación de frecuencia cardíaca
- Sanitización de caracteres especiales

---

# 🖥️ Interfaz

El sistema utiliza:

- Consola ANSI
- Menús interactivos
- Colores para mejorar experiencia de usuario
- Navegación modular

---

# 📌 Estado del Proyecto

✅ Funcional  
✅ Modular  
✅ Persistente  
✅ Escalable  
✅ Validado  

---

# 📄 Licencia

Proyecto académico desarrollado con fines educativos.

# 2. Ejecutar el sistema (Windows)
.\gestion.exe

# 3. Ejecutar el sistema (Linux / macOS)
./gestion
⚠️ Importante: El sistema requiere que exista una carpeta llamada data/ en el mismo directorio donde se ejecuta. Si compilas el código desde cero, asegúrate de crear esta carpeta para que las bases de datos puedan guardarse correctamente.
