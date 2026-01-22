#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "medicos.h"
#include "utils.h"

int buscarMedicoPorCodigo(const Medico medicos[], int cantidad, const char* codigo) {
	for (int i = 0; i < cantidad; i++) {
		if (medicos[i].activo && strcmp(medicos[i].codigo, codigo) == 0) {
			return i;
		}
	}
	return -1;
}

void cargarMedicos(Medico medicos[], int *cantidad) {
	FILE *fp = fopen(ARCHIVO_MEDICOS, "r");
	*cantidad = 0;
	if (!fp) return;
	
	char linea[256];
	while (fgets(linea, sizeof(linea), fp)) {
		if (*cantidad >= MAX_MEDICOS) break;
		
		Medico m;
		int leidos = sscanf(linea, "%[^;];%[^;];%[^;];%[^;];%d;%[^;];%[^;];%d",
							m.codigo,
							m.cedula, 
							m.nombre,
							m.especialidad, 
							&m.edad, 
							m.horario_inicio, 
							m.horario_fin, 
							&m.activo);
		
		if (leidos == 8) {
			medicos[*cantidad] = m;
			(*cantidad)++;
		}
	}
	fclose(fp);
}

void guardarMedicos(const Medico medicos[], int cantidad) {
	FILE *fp = fopen(ARCHIVO_MEDICOS, "w");
	if (!fp) return;
	
	for (int i = 0; i < cantidad; i++) {
		fprintf(fp, "%s;%s;%s;%s;%d;%s;%s;%d\n",
				medicos[i].codigo,
				medicos[i].cedula, 
				medicos[i].nombre,
				medicos[i].especialidad,
				medicos[i].edad,
				medicos[i].horario_inicio,
				medicos[i].horario_fin,
				medicos[i].activo);
	}
	fclose(fp);
}
void inicializarMedicos(Medico medicos[], int *cantidad) { *cantidad = 0; }

void registrarMedico(Medico medicos[], int *cantidad) {
	if (*cantidad >= MAX_MEDICOS) {
		imprimirError("Base de datos de medicos llena.");
		pausar(); return;
	}
	
	Medico m;
	m.activo = 1;
	
	dibujarEncabezado("REGISTRAR NUEVO MEDICO");
	
	printf(NEGRITA "   Perfil Profesional:\n" COLOR_RESET);
	
	// 1. CODIGO 
	do {
		printf("   > Codigo Unico (Ej: DOC01): ");
		leerCadena(m.codigo, 10);
		if (strlen(m.codigo) == 0) {
			imprimirError("El codigo es requerido.");
		} else if (buscarMedicoPorCodigo(medicos, *cantidad, m.codigo) != -1) {
			imprimirError("Ese codigo ya esta en uso.");
			m.codigo[0] = '\0';
		}
	} while (strlen(m.codigo) == 0);
	
	// 2. CEDULA
	do {
		printf("   > Cedula Personal: ");
		leerCadena(m.cedula, 11);
		if (!esCedulaValida(m.cedula)) imprimirError("Cedula invalida.");
	} while (!esCedulaValida(m.cedula));
	
	// 3. NOMBRE
	printf("   > Nombre Completo: ");
	leerCadena(m.nombre, MAX_STR);
	
	// 4. ESPECIALIDAD
	do {
		printf("   > Especialidad: ");
		leerCadena(m.especialidad, MAX_STR);
		if (strlen(m.especialidad) == 0) imprimirError("Especialidad requerida.");
	} while (strlen(m.especialidad) == 0);
	
	m.edad = leerEntero("   > Edad: ");
	
	// 5. HORARIOS
	printf("\n" NEGRITA "   Configuracion de Horario:\n" COLOR_RESET);
	do {
		printf("   > Hora Inicio (HH:MM): ");
		leerCadena(m.horario_inicio, 6);
		if(!esHoraValida(m.horario_inicio)) imprimirError("Formato incorrecto.");
	} while (!esHoraValida(m.horario_inicio));
	
	do {
		printf("   > Hora Fin (HH:MM): ");
		leerCadena(m.horario_fin, 6);
		if(!esHoraValida(m.horario_fin)) imprimirError("Formato incorrecto.");
	} while (!esHoraValida(m.horario_fin));
	
	medicos[*cantidad] = m;
	(*cantidad)++;
	
	barraCarga("Registrando perfil medico");
	imprimirExito("Medico registrado exitosamente.");
	pausar();
}

void listarMedicos(const Medico medicos[], int cantidad) {
	printf("\n");
	printf(COLOR_CYAN "======================================================================\n" COLOR_RESET);
	printf(NEGRITA "%-10s | %-20s | %-15s | %-13s\n" COLOR_RESET, "CODIGO", "NOMBRE", "ESPECIALIDAD", "HORARIO");
	printf(COLOR_CYAN "======================================================================\n" COLOR_RESET);
	
	int encontrados = 0;
	for (int i = 0; i < cantidad; i++) {
		if (medicos[i].activo) {
			printf("%-10s | %-20s | %-15s | %s-%s\n", 
				   medicos[i].codigo, medicos[i].nombre, 
				   medicos[i].especialidad, 
				   medicos[i].horario_inicio, medicos[i].horario_fin);
			encontrados++;
		}
	}
	
	if (encontrados == 0) printf(COLOR_AMARILLO "   (No hay medicos registrados)\n" COLOR_RESET);
	printf(COLOR_CYAN "======================================================================\n" COLOR_RESET);
	pausar();
}

void modificarHorarioMedico(Medico medicos[], int cantidad) {
	char codigo[10];
	dibujarEncabezado("MODIFICAR HORARIO MEDICO");
	
	printf("   > Ingrese Codigo del Medico: ");
	leerCadena(codigo, 10);
	
	int idx = buscarMedicoPorCodigo(medicos, cantidad, codigo);
	
	if (idx == -1) {
		imprimirError("Medico no encontrado.");
	} else {
		printf(COLOR_CYAN "\n   Medico: %s\n" COLOR_RESET, medicos[idx].nombre);
		printf("   Horario Actual: %s - %s\n", medicos[idx].horario_inicio, medicos[idx].horario_fin);
		
		printf("\n" NEGRITA "   Nuevos Valores:\n" COLOR_RESET);
		do {
			printf("   > Nuevo Inicio (HH:MM): ");
			leerCadena(medicos[idx].horario_inicio, 6);
		} while (!esHoraValida(medicos[idx].horario_inicio));
		
		do {
			printf("   > Nuevo Fin (HH:MM): ");
			leerCadena(medicos[idx].horario_fin, 6);
		} while (!esHoraValida(medicos[idx].horario_fin));
		
		imprimirExito("Horario actualizado.");
	}
	pausar();
}
