/* Archivo: auth.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "auth.h"
#include "utils.h"

#define ARCHIVO_USUARIOS "data/usuarios.txt"

// ==========================================
// SINCRONIZADOR DE BASES DE DATOS (.TXT a USUARIOS)
// ==========================================
void sincronizarUsuariosBaseDeDatos(const Paciente pacientes[], int n_pacientes, const Medico medicos[], int n_medicos) {
	FILE *fp = fopen(ARCHIVO_USUARIOS, "r");
	char usuariosExistentes[3000][50]; // Limite alto para evitar desbordes
	int n_usuarios = 0;
	
	// 1. Lee todos los que ya tienen cuenta
	if (fp) {
		char u[50], p[50];
		int r;
		while (fscanf(fp, "%[^;];%[^;];%d\n", u, p, &r) == 3) {
			if (n_usuarios < 3000) {
				strcpy(usuariosExistentes[n_usuarios], u);
				n_usuarios++;
			}
		}
		fclose(fp);
	}
	
	// "a" abre el archivo para agregar lineas al final, sin borrar lo viejo
	fp = fopen(ARCHIVO_USUARIOS, "a"); 
	if (!fp) return;
	
	// 2. Sincronizar Medicos (Les asigna pass: 1234, Rol: 2)
	for (int i = 0; i < n_medicos; i++) {
		if (!medicos[i].activo) continue;
		int existe = 0;
		for (int j = 0; j < n_usuarios; j++) {
			if (strcmp(medicos[i].codigo, usuariosExistentes[j]) == 0) {
				existe = 1; break;
			}
		}
		if (!existe) {
			fprintf(fp, "%s;1234;2\n", medicos[i].codigo);
			if(n_usuarios < 3000) strcpy(usuariosExistentes[n_usuarios++], medicos[i].codigo);
		}
	}
	
	// 3. Sincronizar Pacientes (Les asigna pass: 1234, Rol: 3)
	for (int i = 0; i < n_pacientes; i++) {
		if (!pacientes[i].activo) continue;
		int existe = 0;
		for (int j = 0; j < n_usuarios; j++) {
			if (strcmp(pacientes[i].cedula, usuariosExistentes[j]) == 0) {
				existe = 1; break;
			}
		}
		if (!existe) {
			fprintf(fp, "%s;1234;3\n", pacientes[i].cedula);
			if(n_usuarios < 3000) strcpy(usuariosExistentes[n_usuarios++], pacientes[i].cedula);
		}
	}
	fclose(fp);
}

// ==========================================
// FUNCIONES INTERNAS DE ARCHIVO
// ==========================================
int verificarCredenciales(char* usuario, char* password, int* rolAsignado) {
	FILE *fp = fopen(ARCHIVO_USUARIOS, "r");
	if (!fp) return 0; 
	
	char u[50], p[50];
	int r;
	while (fscanf(fp, "%[^;];%[^;];%d\n", u, p, &r) == 3) {
		if (strcmp(u, usuario) == 0 && strcmp(p, password) == 0) {
			*rolAsignado = r;
			fclose(fp);
			return 1; 
		}
	}
	fclose(fp);
	return 0; 
}

int usuarioExiste(char* usuario) {
	FILE *fp = fopen(ARCHIVO_USUARIOS, "r");
	if (!fp) return 0;
	
	char u[50], p[50];
	int r;
	while (fscanf(fp, "%[^;];%[^;];%d\n", u, p, &r) == 3) {
		if (strcmp(u, usuario) == 0) {
			fclose(fp);
			return 1; 
		}
	}
	fclose(fp);
	return 0;
}

// ==========================================
// REGISTRO DE NUEVOS USUARIOS DESDE EL MENÚ
// ==========================================
void registrarUsuario(Paciente pacientes[], int *n_pacientes, Medico medicos[], int *n_medicos) {
	limpiarPantalla();
	dibujarEncabezado("REGISTRO DE NUEVO USUARIO");
	
	printf("   [1] Soy Administrador\n");
	printf("   [2] Soy Medico\n");
	printf("   [3] Soy Paciente\n");
	int rol = leerEntero("\n   > Seleccione su Rol: ");
	
	if (rol < 1 || rol > 3) {
		imprimirError("Rol invalido.");
		pausar();
		return;
	}
	
	char nuevoUsuario[50];
	char nuevoPassword[50];
	
	printf("\n");
	if (rol == 3) {
		printf("   > Ingrese su numero de CEDULA (sera su usuario): ");
	} else if (rol == 2) {
		printf("   > Ingrese su CODIGO MEDICO (sera su usuario): ");
	} else {
		printf("   > Ingrese un nombre de usuario: ");
	}
	
	leerCadena(nuevoUsuario, 50);
	
	if (usuarioExiste(nuevoUsuario)) {
		imprimirError("Este usuario/cedula ya esta registrado en el sistema.");
		pausar();
		return;
	}
	
	printf("   > Cree una contrasena: ");
	leerCadena(nuevoPassword, 50);
	
	if (rol == 3) {
		if (*n_pacientes >= MAX_PACIENTES) {
			imprimirError("Base de datos de pacientes llena.");
			pausar(); return;
		}
		printf(COLOR_CYAN "\n   --- COMPLETE SU FICHA DE PACIENTE ---\n" COLOR_RESET);
		Paciente p;
		strcpy(p.cedula, nuevoUsuario);
		printf("   > Nombre Completo: "); leerCadena(p.nombre, 50);
		p.edad = leerEntero("   > Edad: ");
		printf("   > Telefono: "); leerCadena(p.telefono, 15);
		printf("   > Correo Electronico: "); leerCadena(p.email, 50); 
		p.activo = 1;
		
		pacientes[*n_pacientes] = p;
		(*n_pacientes)++;
		guardarPacientes(pacientes, *n_pacientes);
		printf(COLOR_VERDE "   >> Ficha clinica creada exitosamente.\n" COLOR_RESET);
	} 
	else if (rol == 2) {
		if (*n_medicos >= MAX_MEDICOS) {
			imprimirError("Base de datos de medicos llena.");
			pausar(); return;
		}
		printf(COLOR_CYAN "\n   --- COMPLETE SU PERFIL MEDICO ---\n" COLOR_RESET);
		Medico m;
		strcpy(m.codigo, nuevoUsuario);
		printf("   > Nombre (Ej: Dr. Juan Perez): "); leerCadena(m.nombre, 50);
		printf("   > Especialidad: "); leerCadena(m.especialidad, 50);
		
		// --- ESTO ES LO NUEVO ---
		char bufferPrecio[20];
		printf("   > Tarifa de su consulta ($): "); 
		leerCadena(bufferPrecio, 20);
		m.precio_consulta = atof(bufferPrecio);
		// ------------------------
		
		strcpy(m.horario_inicio, "08:00"); 
		strcpy(m.horario_fin, "16:00");
		m.activo = 1;
		
		medicos[*n_medicos] = m;
		(*n_medicos)++;
		guardarMedicos(medicos, *n_medicos);
		printf(COLOR_VERDE "   >> Perfil medico creado exitosamente.\n" COLOR_RESET);
	}
	
	FILE *fp = fopen(ARCHIVO_USUARIOS, "a");
	if (fp) {
		fprintf(fp, "%s;%s;%d\n", nuevoUsuario, nuevoPassword, rol);
		fclose(fp);
		barraCarga("Registrando cuenta");
		imprimirExito("¡Usuario registrado con exito! Ya puede iniciar sesion.");
	} else {
		imprimirError("Error critico: No se pudo escribir en usuarios.txt");
	}
	pausar();
}

// ==========================================
// FLUJO PRINCIPAL DE AUTENTICACIÓN
// ==========================================
int sistemaAutenticacion(char* usuarioActual, Paciente pacientes[], int *n_pacientes, Medico medicos[], int *n_medicos) {
	int opcion;
	do {
		limpiarPantalla();
		dibujarEncabezado("CLINICA - ACCESO AL SISTEMA");
		printf("   [1] Iniciar Sesion\n");
		printf("   [2] Registrarse (Nuevos Usuarios)\n");
		printf(COLOR_ROJO "   [0] Salir del Sistema\n" COLOR_RESET);
		opcion = leerEntero("\n   > Seleccione una opcion: ");
		
		if (opcion == 1) {
			char user[50], pass[50];
			int rol = 0;
			
			printf("\n   > Usuario (Cedula/Codigo): ");
			leerCadena(user, 50);
			printf("   > Contrasena: ");
			leerCadena(pass, 50);
			
			if (strcmp(user, "admin") == 0 && strcmp(pass, "admin") == 0) {
				strcpy(usuarioActual, "admin");
				barraCarga("Validando credenciales");
				return ROL_ADMIN;
			}
			
			if (verificarCredenciales(user, pass, &rol)) {
				strcpy(usuarioActual, user);
				barraCarga("Validando credenciales");
				return rol;
			} else {
				imprimirError("Usuario o contrasena incorrectos.");
				pausar();
			}
		} 
		else if (opcion == 2) {
			registrarUsuario(pacientes, n_pacientes, medicos, n_medicos);
		}
	} while (opcion != 0);
	
	return 0; 
}
