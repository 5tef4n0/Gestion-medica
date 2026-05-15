/* Archivo: pacientes.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pacientes.h"
#include "utils.h"

void inicializarPacientes(Paciente pacientes[], int *cantidad) {
	*cantidad = 0;
}

void cargarPacientes(Paciente pacientes[], int *cantidad) {
	FILE *fp = fopen(ARCHIVO_PACIENTES, "r");
	*cantidad = 0;
	if (!fp) return;
	
	char linea[256];
	while (fgets(linea, sizeof(linea), fp)) {
		if (*cantidad >= MAX_PACIENTES) break;
		Paciente p;
		
		// Limpiamos los saltos de linea
		linea[strcspn(linea, "\r\n")] = 0;
		
		// Leemos 6 campos: cedula;nombre;edad;telefono;email;activo
		int leidos = sscanf(linea, "%[^;];%[^;];%d;%[^;];%[^;];%d", 
							p.cedula, p.nombre, &p.edad, p.telefono, p.email, &p.activo);
		
		// Si el archivo era el formato viejo (sin email), le asignamos uno por defecto
		if (leidos == 5) {
			sscanf(linea, "%[^;];%[^;];%d;%[^;];%d", p.cedula, p.nombre, &p.edad, p.telefono, &p.activo);
			strcpy(p.email, "sin_correo@mail.com");
			leidos = 6;
		}
		
		if (leidos >= 6) {
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
				pacientes[i].cedula,
				pacientes[i].nombre,
				pacientes[i].edad,
				pacientes[i].telefono,
				pacientes[i].email,
				pacientes[i].activo);
	}
	fclose(fp);
}

void registrarPaciente(Paciente pacientes[], int *cantidad) {
	if (*cantidad >= MAX_PACIENTES) {
		imprimirError("Se alcanzo el limite maximo de pacientes.");
		pausar();
		return;
	}
	
	Paciente nuevo;
	printf("\n   > Cedula (Ej: 1700000001): ");
	leerCadena(nuevo.cedula, 15);
	
	if (buscarPacientePorCedula(pacientes, *cantidad, nuevo.cedula) != -1) {
		imprimirError("Ya existe un paciente con esa cedula.");
		pausar();
		return;
	}
	
	printf("   > Nombre Completo: ");
	leerCadena(nuevo.nombre, 50);
	
	nuevo.edad = leerEntero("   > Edad: ");
	
	printf("   > Telefono: ");
	leerCadena(nuevo.telefono, 15);
	
	printf("   > Correo Electronico: ");
	leerCadena(nuevo.email, 50);
	
	nuevo.activo = 1;
	
	pacientes[*cantidad] = nuevo;
	(*cantidad)++;
	
	guardarPacientes(pacientes, *cantidad);
	
	barraCarga("Guardando paciente");
	imprimirExito("Paciente registrado correctamente.");
	pausar();
}

void listarPacientes(const Paciente pacientes[], int cantidad) {
	printf("\n");
	printf(COLOR_CYAN "========================================================================================\n" COLOR_RESET);
	printf(NEGRITA "%-15s | %-25s | %-4s | %-12s | %-20s\n" COLOR_RESET, "CEDULA", "NOMBRE", "EDAD", "TELEFONO", "EMAIL");
	printf(COLOR_CYAN "========================================================================================\n" COLOR_RESET);
	
	int activos = 0;
	for (int i = 0; i < cantidad; i++) {
		if (pacientes[i].activo) {
			printf("%-15s | %-25s | %-4d | %-12s | %-20s\n",
				   pacientes[i].cedula, pacientes[i].nombre, pacientes[i].edad, pacientes[i].telefono, pacientes[i].email);
			activos++;
		}
	}
	
	if (activos == 0) {
		printf(COLOR_AMARILLO "No hay pacientes registrados en el sistema.\n" COLOR_RESET);
	}
	printf(COLOR_CYAN "========================================================================================\n" COLOR_RESET);
	pausar();
}

int buscarPacientePorCedula(const Paciente pacientes[], int cantidad, const char *cedula) {
	for (int i = 0; i < cantidad; i++) {
		if (strcmp(pacientes[i].cedula, cedula) == 0 && pacientes[i].activo) {
			return i;
		}
	}
	return -1;
}
