#include <stdio.h>
#include <string.h>
#include "reportes.h"
#include "utils.h"

void reportePacientesRecurrentes(const Cita citas[], int n_citas, const Paciente pacientes[], int n_pacientes) {
	dibujarEncabezado("REPORTE: PACIENTES RECURRENTES");
	printf(NEGRITA "   %-12s | %-25s | %-10s\n" COLOR_RESET, "CEDULA", "NOMBRE", "CANTIDAD");
	imprimirLinea(60);
	
	int encontrados = 0;
	
	for (int i = 0; i < n_pacientes; i++) {
		if (!pacientes[i].activo) continue;
		
		int conteo = 0;
		for (int j = 0; j < n_citas; j++) {
			if (citas[j].estado == ESTADO_ACTIVA && 
				strcmp(citas[j].cedula_paciente, pacientes[i].cedula) == 0) {
				conteo++;
			}
		}
		
		if (conteo > 1) {
			printf("   %-12s | %-25s | " COLOR_AMARILLO "%-10d" COLOR_RESET "\n", 
				   pacientes[i].cedula, pacientes[i].nombre, conteo);
			encontrados++;
		}
	}
	
	if (encontrados == 0) {
		printf(COLOR_CYAN "   No hay pacientes con multiples citas activas.\n" COLOR_RESET);
	}
	imprimirLinea(60);
	pausar();
}

void reporteMedicosMasDemandados(const Cita citas[], int n_citas, const Medico medicos[], int n_medicos) {
	dibujarEncabezado("REPORTE: DEMANDA DE MEDICOS");
	
	typedef struct {
		int indiceMedico;
		int totalCitas;
	} Ranking;
	
	Ranking ranking[MAX_MEDICOS];
	int totalMedicosActivos = 0;
	
	// 1. Contar citas
	for (int i = 0; i < n_medicos; i++) {
		if (!medicos[i].activo) continue;
		
		ranking[totalMedicosActivos].indiceMedico = i;
		ranking[totalMedicosActivos].totalCitas = 0;
		
		for (int j = 0; j < n_citas; j++) {
			if (citas[j].estado == ESTADO_ACTIVA && 
				strcmp(citas[j].codigo_medico, medicos[i].codigo) == 0) {
				ranking[totalMedicosActivos].totalCitas++;
			}
		}
		totalMedicosActivos++;
	}
	
	// 2. Ordenar 
	for (int i = 0; i < totalMedicosActivos - 1; i++) {
		for (int j = 0; j < totalMedicosActivos - i - 1; j++) {
			if (ranking[j].totalCitas < ranking[j + 1].totalCitas) {
				Ranking temp = ranking[j];
				ranking[j] = ranking[j + 1];
				ranking[j + 1] = temp;
			}
		}
	}
	
	// 3. Imprimir
	printf(NEGRITA "   %-25s | %-10s\n" COLOR_RESET, "MEDICO", "TOTAL CITAS");
	imprimirLinea(60);
	
	for (int i = 0; i < totalMedicosActivos; i++) {
		int idx = ranking[i].indiceMedico;
		int cantidad = ranking[i].totalCitas;
		
		if (cantidad > 0) {
			printf("   %-25s | " COLOR_VERDE "%-10d" COLOR_RESET "\n", 
				   medicos[idx].nombre, cantidad);
		}
	}
	imprimirLinea(60);
	pausar();
}

void reporteResumenCitasPorMedico(const Cita citas[], int n_citas, const Medico medicos[], int n_medicos) {
	dibujarEncabezado("RESUMEN: CITAS POR MEDICO");
	
	for(int i=0; i<n_medicos; i++) {
		if(!medicos[i].activo) continue;
		
		printf(NEGRITA COLOR_CYAN "   DR/A. %s (%s)\n" COLOR_RESET, 
			   medicos[i].nombre, medicos[i].especialidad);
		
		int tieneCitas = 0;
		for(int j=0; j<n_citas; j++) {
			if(strcmp(citas[j].codigo_medico, medicos[i].codigo) == 0 && citas[j].estado == ESTADO_ACTIVA) {
				printf("     -> %s %s - Paciente: %s\n", 
					   citas[j].fecha, citas[j].hora, citas[j].cedula_paciente);
				tieneCitas = 1;
			}
		}
		
		if(!tieneCitas) printf("     (Sin citas asignadas)\n");
		printf("\n");
	}
	imprimirLinea(60);
	pausar();
}

// --- MENÚ DE REPORTES ---
void menuReportes(const Cita citas[], int n_citas, 
				  const Paciente pacientes[], int n_pacientes, 
				  const Medico medicos[], int n_medicos) {
	int op = -1;
	while(op != 0) {
		dibujarEncabezado("MODULO DE REPORTES");
		printf(COLOR_VERDE "   [1]" COLOR_RESET " Pacientes Recurrentes \n");
		printf(COLOR_VERDE "   [2]" COLOR_RESET " Medicos Frecuentados\n");
		printf(COLOR_VERDE "   [3]" COLOR_RESET " Resumen de Citas por Medico\n");
		printf(COLOR_ROJO  "   [0]" COLOR_RESET " Volver al Menu Principal\n");
		imprimirLinea(60);
		op = leerEntero("   > Seleccione reporte: ");
		
		switch (op) {
		case 1: reportePacientesRecurrentes(citas, n_citas, pacientes, n_pacientes); break;
		case 2: reporteMedicosMasDemandados(citas, n_citas, medicos, n_medicos); break;
		case 3: reporteResumenCitasPorMedico(citas, n_citas, medicos, n_medicos); break;
		case 0: break;
		default: imprimirError("Opcion invalida."); pausar();
		}
	}
}
