#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "auth.h"
#include "utils.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define ARCHIVO_USUARIOS "data/usuarios.txt"

typedef struct {
	char username[MAX_STR];
	char password[MAX_STR];
	int rol;
} DatosUsuario;

void guardarUsuarioEnArchivo(const char* user, const char* pass, int rol) {
	FILE *fp = fopen(ARCHIVO_USUARIOS, "a");
	if (fp) {
		fprintf(fp, "%s;%s;%d\n", user, pass, rol);
		fclose(fp);
	}
}

int validarCredenciales(char* user, char* pass, int* rolDetectado) {
	FILE *fp = fopen(ARCHIVO_USUARIOS, "r");
	if (!fp) return 0;
	
	char linea[256];
	char u[MAX_STR], p[MAX_STR];
	int r;
	
	while (fgets(linea, sizeof(linea), fp)) {
		sscanf(linea, "%[^;];%[^;];%d", u, p, &r);
		if (strcmp(user, u) == 0 && strcmp(pass, p) == 0) {
			*rolDetectado = r;
			fclose(fp);
			return 1;
		}
	}
	fclose(fp);
	return 0;
}

int existeUsuario(const char* user) {
	FILE *fp = fopen(ARCHIVO_USUARIOS, "r");
	if (!fp) return 0;
	
	char linea[256], u[MAX_STR], p[MAX_STR];
	int r;
	while (fgets(linea, sizeof(linea), fp)) {
		sscanf(linea, "%[^;];%[^;];%d", u, p, &r);
		if (strcmp(user, u) == 0) {
			fclose(fp);
			return 1;
		}
	}
	fclose(fp);
	return 0;
}


void registrarUsuario() {
	char user[MAX_STR], pass[MAX_STR];
	int rol = -1;
	
	dibujarEncabezado("REGISTRO DE NUEVO USUARIO");
	
	// 1.Rol
	printf(NEGRITA "   Paso 1: Seleccione su Perfil\n" COLOR_RESET);
	imprimirLinea(60);
	printf(COLOR_CYAN "   [1]" COLOR_RESET " Administrador \n");
	printf(COLOR_CYAN "   [2]" COLOR_RESET " Medico \n");
	printf(COLOR_CYAN "   [3]" COLOR_RESET " Paciente \n");
	imprimirLinea(60);
	
	do {
		rol = leerEntero("   > Seleccione una opcion (1-3): ");
		if (rol < 1 || rol > 3) imprimirError("Opcion invalida.");
	} while (rol < 1 || rol > 3);
	
	// 2.Usuario
	printf("\n" NEGRITA "   Paso 2:Identificacion\n" COLOR_RESET);
	
	int valido = 0;
	do {
		// Según el rol
		if (rol == 3) {
			printf("   > Ingrese su cedula: ");
		} else if (rol == 2) {
			printf("   > Ingrese su codigo medico (Ej: DOC01): ");
		} else {
			printf("   > Ingrese su nombre de usuario: ");
		}
		
		leerCadena(user, MAX_STR);
		
		if (existeUsuario(user)) {
			imprimirError("Este usuario/cedula ya esta registrado.");
			valido = 0;
		} 
		else if (rol == 3) { 
			if (!esCedulaValida(user)) {
				imprimirError("Cedula invalida. Debe tener 10 digitos numericos.");
				valido = 0;
			} else {
				valido = 1;
			}
		} 
		else { 
			if (strlen(user) < 3) {
				imprimirError("El usuario/codigo debe tener al menos 3 caracteres.");
				valido = 0;
			} else {
				valido = 1;
			}
		}
	} while (!valido);
	
	// 3. Contraseña
	do {
		printf("   > Contrasena: ");
		leerCadena(pass, MAX_STR);
		if (strlen(pass) < 3) imprimirError("Contrasena muy corta.");
	} while (strlen(pass) < 3);
	
	// Guardar
	guardarUsuarioEnArchivo(user, pass, rol);
	
	barraCarga("Configurando perfil");
	if(rol == 3) imprimirExito("Paciente registrado. Use su cedula para entrar.");
	else if(rol == 2) imprimirExito("Medico registrado. Use su codigo para entrar.");
	else imprimirExito("Administrador registrado correctamente.");
	
	pausar();
}

// LOGIN

int iniciarSesion(char* usuarioRetorno) {
	char user[MAX_STR], pass[MAX_STR];
	int rol = -1;
	
	dibujarEncabezado("INICIAR SESION");
	
	printf(COLOR_CYAN "   Nota: Pacientes usar cedula, Medicos usar codigo.\n" COLOR_RESET);
	printf("   > Usuario/Cedula: ");
	leerCadena(user, MAX_STR);
	
	printf("   > Contrasena: ");
	leerCadena(pass, MAX_STR);
	
	printf("\n");
#ifdef _WIN32
	Sleep(500);
#else
	usleep(500000);
#endif
	
	if (validarCredenciales(user, pass, &rol)) {
		strcpy(usuarioRetorno, user);
		imprimirExito("Credenciales correctas.");
#ifdef _WIN32
		Sleep(800);
#else
		usleep(800000);
#endif
		return rol;
	} else {
		imprimirError("Credenciales incorrectas.");
		pausar();
		return -1;
	}
}


int sistemaAutenticacion(char* usuarioActual) {
	int opcion = -1;
	
	while (1) {
		dibujarEncabezado("SISTEMA DE GESTION MEDICA");
		printf("\n");
		imprimirTextoCentrado("BIENVENIDO AL SISTEMA", NEGRITA);
		printf("\n");
		
		imprimirOpcionCentrada("1", "Iniciar Sesion");
		imprimirOpcionCentrada("2", "Registrarse");
		
		printf("\n");
		int lenSalida = strlen("[0] Salir del Sistema");
		int padSalida = (60 - lenSalida) / 2;
		for(int i=0; i<padSalida; i++) printf(" ");
		printf(COLOR_ROJO "[0]" COLOR_RESET " Salir del Sistema\n");
		
		imprimirLinea(60);
		opcion = leerEntero("   > Seleccione una opcion: ");
		
		if (opcion == 1) {
			int rol = iniciarSesion(usuarioActual);
			if (rol != -1) return rol;
		}
		else if (opcion == 2) {
			registrarUsuario();
		}
		else if (opcion == 0) {
			printf(COLOR_ROJO "\n   Cerrando aplicacion...\n" COLOR_RESET);
			exit(0);
		}
		else {
			imprimirError("Opcion invalida.");
			pausar();
		}
	}
}
