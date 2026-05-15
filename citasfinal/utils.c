#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

#ifdef _WIN32
#include <windows.h>
#define SLEEP_MS(x) Sleep(x)
#else
#include <unistd.h>
#define SLEEP_MS(x) usleep(x * 1000)
#endif


void limpiarPantalla() {
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
}

void imprimirLinea(int longitud) {
	printf(COLOR_CYAN);
	for(int i=0; i<longitud; i++) printf("=");
	printf(COLOR_RESET "\n");
}

void dibujarEncabezado(const char* titulo) {
	limpiarPantalla();
	printf("\n");
	imprimirLinea(60);
	int espacios = (60 - strlen(titulo)) / 2;
	printf(COLOR_AZUL "|");
	for(int i=0; i<espacios-1; i++) printf(" ");
	printf(NEGRITA COLOR_AMARILLO "%s" COLOR_RESET, titulo);
	for(int i=0; i<espacios-1; i++) printf(" ");
	printf(COLOR_AZUL "|\n" COLOR_RESET);
	imprimirLinea(60);
	printf("\n");
}

void barraCarga(const char* mensaje) {
	printf("\n%s " COLOR_AMARILLO, mensaje);
	for(int i=0; i<20; i++) {
		printf("-");
		fflush(stdout);
		SLEEP_MS(30); 
	}
	printf(COLOR_VERDE " OK!" COLOR_RESET "\n");
	SLEEP_MS(200);
}

void imprimirExito(const char* msj) {
	printf(COLOR_VERDE ">> EXITO: %s" COLOR_RESET "\n", msj);
}

void imprimirError(const char* msj) {
	printf(COLOR_ROJO ">> ERROR: %s" COLOR_RESET "\n", msj);
}

void leerCadena(char *cadena, int longitud) {
	fflush(stdin);
	fgets(cadena, longitud, stdin);
	size_t len = strlen(cadena);
	if (len > 0 && cadena[len - 1] == '\n') {
		cadena[len - 1] = '\0';
	}
}

int leerEntero(const char* mensaje) {
	int valor;
	char buffer[100];
	printf(COLOR_BLANCO "%s" COLOR_RESET, mensaje);
	leerCadena(buffer, 100);
	valor = atoi(buffer);
	return valor;
}

void pausar() {
	printf("\n" COLOR_MAGENTA "Presione ENTER para continuar..." COLOR_RESET);
	getchar();
}

bool esCedulaValida(const char* cedula) {
	if (!cedula || strlen(cedula) != 10) return false;
	for (int i = 0; i < 10; i++) if (!isdigit(cedula[i])) return false;
	return true;
}

bool esCorreoValido(const char* correo) {
	if (strchr(correo, '@') && strchr(correo, '.')) return true;
	return false;
}

bool esFechaValida(const char* fecha) {
	if (strlen(fecha) != 10) return false;
	return (fecha[2] == '/' && fecha[5] == '/');
}

bool esHoraValida(const char* hora) {
	if (strlen(hora) != 5) return false;
	return (hora[2] == ':');
}

void imprimirTextoCentrado(const char* texto, const char* color) {
	int anchoTotal = 60;
	int len = strlen(texto);
	int padding = (anchoTotal - len) / 2;
	
	if (padding < 0) padding = 0;
	
	for(int i=0; i<padding; i++) printf(" ");
	
	printf("%s%s%s\n", color, texto, COLOR_RESET);
}

void imprimirOpcionCentrada(const char* num, const char* texto) {
	int anchoTotal = 60;
	int lenVisible = strlen(num) + 3 + strlen(texto); 
	int padding = (anchoTotal - lenVisible) / 2;
	
	if (padding < 0) padding = 0;
	
	for(int i=0; i<padding; i++) printf(" ");
	
	printf(COLOR_VERDE "[%s]" COLOR_RESET " %s\n", num, texto);
}
