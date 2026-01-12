#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pacientes.h"
#include "utils.h"

int buscarPacientePorCedula(const Paciente pacientes[], int cantidad, const char* cedula) {
	for (int i = 0; i < cantidad; i++) {
		if (pacientes[i].activo && strcmp(pacientes[i].cedula, cedula) == 0) {
			return i; 
		}
	}
	return -1; 
}

void cargarPacientes(Paciente pacientes[], int *cantidad) {
	FILE *fp = fopen(ARCHIVO_PACIENTES, "r");
	*cantidad = 0;
	if (!fp) return;
	
	char linea[256];
	while (fgets(linea, sizeof(linea), fp)) {
		if (*cantidad >= MAX_PACIENTES) break;
		Paciente p;
		int leidos = sscanf(linea, "%[^;];%[^;];%d;%[^;];%[^;];%d",
							p.cedula, p.nombre, &p.edad, 
							p.telefono, p.correo, &p.activo);
		if (leidos == 6) {
			pacientes[*cantidad] = p;
			(*cantidad)++;
		}
	}
	fclose(fp);
}

void guardarPacientes(const Paciente pacientes[], int cantidad) {
	FILE *fp = fopen(ARCHIVO_PACIENTES, "w");
	if (!fp) return;
	for (int i = 0; i < cantidad; i++) {
		fprintf(fp, "%s;%s;%d;%s;%s;%d\n",
				pacientes[i].cedula, pacientes[i].nombre, pacientes[i].edad,
				pacientes[i].telefono, pacientes[i].correo, pacientes[i].activo);
	}
	fclose(fp);
}

void inicializarPacientes(Paciente pacientes[], int *cantidad) { *cantidad = 0; }

void registrarPaciente(Paciente pacientes[], int *cantidad) {
	if (*cantidad >= MAX_PACIENTES) {
		imprimirError("Base de datos de pacientes llena.");
		pausar(); return;
	}
	
	Paciente p;
	p.activo = 1;
	
	dibujarEncabezado("REGISTRAR NUEVO PACIENTE");
	
	// 1.ID
	printf(NEGRITA "   Datos Personales:\n" COLOR_RESET);
	do {
		printf("   > Nombre Completo: ");
		leerCadena(p.nombre, MAX_STR);
		if (strlen(p.nombre) < 3) imprimirError("Nombre muy corto.");
	} while (strlen(p.nombre) < 3);
	
	do {
		printf("   > Cedula: ");
		leerCadena(p.cedula, 11);
		
		if (!esCedulaValida(p.cedula)) {
			imprimirError("Cedula invalida (solo numeros).");
		} else if (buscarPacientePorCedula(pacientes, *cantidad, p.cedula) != -1) {
			imprimirError("Esa cedula ya esta registrada.");
			p.cedula[0] = '\0';
		}
	} while (!esCedulaValida(p.cedula));
	
	do {
		p.edad = leerEntero("   > Edad: ");
		if (p.edad < 0 || p.edad > 120) imprimirError("Edad imposible.");
	} while (p.edad < 0 || p.edad > 120);
	
	// 2.Contacto
	printf("\n" NEGRITA "   Datos de Contacto:\n" COLOR_RESET);
	do {
		printf("   > Telefono: ");
		leerCadena(p.telefono, 15);
		if (strlen(p.telefono) < 7) imprimirError("Telefono invalido.");
	} while (strlen(p.telefono) < 7);
	
	do {
		printf("   > Correo: ");
		leerCadena(p.correo, MAX_STR);
		if (!esCorreoValido(p.correo)) imprimirError("Falta '@' o '.'");
	} while (!esCorreoValido(p.correo));
	
	pacientes[*cantidad] = p;
	(*cantidad)++;
	
	barraCarga("Guardando en el sistema");
	imprimirExito("Paciente registrado correctamente.");
	pausar();
}

void listarPacientes(const Paciente pacientes[], int cantidad) {
	printf("\n");
	printf(COLOR_CYAN "======================================================================\n" COLOR_RESET);
	printf(NEGRITA "%-12s | %-25s | %-5s | %-15s\n" COLOR_RESET, "CEDULA", "NOMBRE", "EDAD", "TELEFONO");
	printf(COLOR_CYAN "======================================================================\n" COLOR_RESET);
	
	int encontrados = 0;
	for (int i = 0; i < cantidad; i++) {
		if (pacientes[i].activo) {
			printf("%-12s | %-25s | %-5d | %-15s\n", 
				   pacientes[i].cedula, pacientes[i].nombre, 
				   pacientes[i].edad, pacientes[i].telefono);
			encontrados++;
		}
	}
	
	if (encontrados == 0) printf(COLOR_AMARILLO "   (No hay pacientes registrados)\n" COLOR_RESET);
	printf(COLOR_CYAN "======================================================================\n" COLOR_RESET);
	pausar();
}
