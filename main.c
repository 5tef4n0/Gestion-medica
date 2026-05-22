/* Archivo: main.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "utils.h"
#include "auth.h"
#include "pacientes.h"
#include "medicos.h"
#include "citas.h"
#include "reportes.h"

int main() {
	static Paciente pacientes[MAX_PACIENTES];
	static Medico medicos[MAX_MEDICOS];
	static Cita citas[MAX_CITAS];
	
	int n_pacientes = 0, n_medicos = 0, n_citas = 0;
	
	inicializarPacientes(pacientes, &n_pacientes);
	inicializarMedicos(medicos, &n_medicos);
	inicializarCitas(citas, &n_citas);
	
	// 1. Cargar datos de los archivos .txt a la RAM
	cargarPacientes(pacientes, &n_pacientes);
	cargarMedicos(medicos, &n_medicos);
	cargarCitas(citas, &n_citas);
	
	// 2. NUEVO: Ejecutar el Sincronizador Mágico
	// Si metiste pacientes o médicos directo en el .txt, aquí les crea contraseña "1234"
	sincronizarUsuariosBaseDeDatos(pacientes, n_pacientes, medicos, n_medicos);
	
	char usuarioActual[MAX_STR];
	int rolActual = 0;
	int opcion;
	
	while (1) { 
		limpiarPantalla();
		rolActual = sistemaAutenticacion(usuarioActual, pacientes, &n_pacientes, medicos, &n_medicos);
		
		if (rolActual == 0) break; 
		
		do {
			limpiarPantalla();
			
			// ==========================================
			// MENÚ ADMINISTRADOR
			// ==========================================
			if (rolActual == ROL_ADMIN) {
				dibujarEncabezado("PANEL DE ADMINISTRADOR");
				printf("   [1] Gestionar Pacientes\n");
				printf("   [2] Gestionar Medicos\n");
				printf("   [3] Gestionar Citas\n");
				printf("   [4] Reportes y Estadisticas\n");
				printf(COLOR_ROJO "   [0] Cerrar Sesion\n" COLOR_RESET);
				opcion = leerEntero("\n   > Seleccione una opcion: ");
				
				switch (opcion) {
				case 1:
					limpiarPantalla();
					dibujarEncabezado("GESTION DE PACIENTES");
					printf("   [1] Registrar Paciente\n");
					printf("   [2] Listar Pacientes\n");
					int opPac = leerEntero("\n   > Opcion: ");
					if (opPac == 1) registrarPaciente(pacientes, &n_pacientes);
					else if (opPac == 2) listarPacientes(pacientes, n_pacientes);
					break;
				case 2:
					limpiarPantalla();
					dibujarEncabezado("GESTION DE MEDICOS");
					printf("   [1] Registrar Medico\n");
					printf("   [2] Modificar Horario\n");
					printf("   [3] Listar Medicos\n");
					int opMed = leerEntero("\n   > Opcion: ");
					if (opMed == 1) registrarMedico(medicos, &n_medicos);
					else if (opMed == 2) modificarHorarioMedico(medicos, n_medicos);
					else if (opMed == 3) listarMedicos(medicos, n_medicos);
					break;
				case 3:
					limpiarPantalla();
					dibujarEncabezado("GESTION DE CITAS");
					printf("   [1] Agendar Cita\n");
					printf("   [2] Reagendar Cita\n");
					printf("   [3] Cancelar Cita\n");
					printf("   [4] Listar Todas las Citas\n");
					int opCita = leerEntero("\n   > Opcion: ");
					if (opCita == 1) agendarCita(citas, &n_citas, pacientes, n_pacientes, medicos, n_medicos, rolActual, usuarioActual);
					else if (opCita == 2) reagendarCita(citas, n_citas, medicos, n_medicos, rolActual, usuarioActual);
					else if (opCita == 3) cancelarCita(citas, n_citas, rolActual, usuarioActual);
					else if (opCita == 4) listarCitas(citas, n_citas);
					break;
				case 4:
					menuReportes(citas, n_citas, pacientes, n_pacientes, medicos, n_medicos);
					break;
				case 0: break;
				default: imprimirError("Opcion no valida."); pausar();
				}
			} 
			
			// ==========================================
			// MENÚ MÉDICO
			// ==========================================
			else if (rolActual == ROL_MEDICO) {
				dibujarEncabezado("PANEL DE MEDICO");
				printf(COLOR_CYAN "   Bienvenido Dr/a: %s\n\n" COLOR_RESET, usuarioActual);
				printf("   [1] Ver mi agenda e historial clinico\n"); 
				printf("   [2] Agendar Nueva Cita a Paciente\n"); 
				printf("   [3] Atender Paciente en Consultorio\n"); 
				printf("   [4] Modificar mi horario laboral\n");
				printf("   [5] Registrar Ausencia de paciente\n");
				printf("   [6] Modificar Tarifa de Consulta\n"); // <-- NUEVA OPCION
				printf(COLOR_ROJO "   [0] Cerrar Sesion\n" COLOR_RESET);
				opcion = leerEntero("\n   > Seleccione una opcion: ");
				
				switch (opcion) {
				case 1:
					verCitasDeMedico(citas, n_citas, pacientes, n_pacientes, medicos, n_medicos, usuarioActual);
					break;
				case 2:
					agendarCita(citas, &n_citas, pacientes, n_pacientes, medicos, n_medicos, rolActual, usuarioActual);
					break;
				case 3:
					atenderPaciente(citas, n_citas, pacientes, n_pacientes, usuarioActual);
					break;
				case 4:
					modificarHorarioMedico(medicos, n_medicos);
					break;
				case 5:
					registrarAusencia(citas, n_citas, rolActual, usuarioActual);
					break;
				case 6: // <-- NUEVO CASE
					modificarPrecioConsulta(medicos, n_medicos, usuarioActual);
					break;
				case 0: break;
				default: imprimirError("Opcion no valida."); pausar();
				}
			}
			
			// ==========================================
			// MENÚ PACIENTE
			// ==========================================
			else if (rolActual == ROL_PACIENTE) {
				dibujarEncabezado("PANEL DE PACIENTE");
				printf(COLOR_CYAN "   C.I: %s\n\n" COLOR_RESET, usuarioActual);
				printf("   [1] Agendar Nueva Cita\n");
				printf("   [2] Reagendar mi Cita\n");
				printf("   [3] Cancelar mi Cita\n");
				printf("   [4] Ver Mis Citas e Historial Medico\n"); 
				printf(COLOR_ROJO "   [0] Cerrar Sesion\n" COLOR_RESET);
				opcion = leerEntero("\n   > Seleccione una opcion: ");
				
				switch (opcion) {
				case 1:
					agendarCita(citas, &n_citas, pacientes, n_pacientes, medicos, n_medicos, rolActual, usuarioActual);
					break;
				case 2:
					reagendarCita(citas, n_citas, medicos, n_medicos, rolActual, usuarioActual);
					break;
				case 3:
					cancelarCita(citas, n_citas, rolActual, usuarioActual);
					break;
				case 4:
					verHistorialPaciente(citas, n_citas, pacientes, n_pacientes, medicos, n_medicos, usuarioActual);
					break;
				case 0: break;
				default: imprimirError("Opcion no valida."); pausar();
				}
			}
			
			// Guardado Automático
			guardarPacientes(pacientes, n_pacientes);
			guardarMedicos(medicos, n_medicos);
			guardarCitas(citas, n_citas);
			
		} while (opcion != 0); 
	}
	
	limpiarPantalla();
	imprimirTextoCentrado("Guardando datos...", COLOR_AMARILLO);
	guardarPacientes(pacientes, n_pacientes);
	guardarMedicos(medicos, n_medicos);
	guardarCitas(citas, n_citas);
	printf("\n"); imprimirTextoCentrado("Gracias por usar el Sistema de Gestion Medica", COLOR_VERDE); printf("\n\n");
	return 0;
}
