#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

// Colores y estilos
#define COLOR_RESET   "\x1b[0m"
#define COLOR_ROJO    "\x1b[31m"
#define COLOR_VERDE   "\x1b[32m"
#define COLOR_AMARILLO "\x1b[33m"
#define COLOR_AZUL    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_BLANCO  "\x1b[37m"
#define NEGRITA       "\x1b[1m"
#define FONDO_AZUL    "\x1b[44m"


void imprimirTextoCentrado(const char* texto, const char* color);
void imprimirOpcionCentrada(const char* num, const char* texto);
// Funciones de Entrada
void leerCadena(char *cadena, int longitud);
int leerEntero(const char* mensaje);
void pausar();
void limpiarPantalla();

// Funciones Visuales
void dibujarEncabezado(const char* titulo);
void barraCarga(const char* mensaje);
void imprimirLinea(int longitud);
void imprimirExito(const char* msj);
void imprimirError(const char* msj);

// Validaciones
bool esCedulaValida(const char* cedula);
bool esCorreoValido(const char* correo);
bool esFechaValida(const char* fecha);
bool esHoraValida(const char* hora);

#endif
