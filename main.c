#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include "config.h"
#include "pacientes.h"
#include "medicos.h"
#include "citas.h"
#include "auth.h"
#include "utils.h"
#include "reportes.h"

#ifdef _WIN32
#define strcasecmp _stricmp
#endif
Paciente g_pacientes[MAX_PACIENTES];
int n_pacientes = 0;

Medico g_medicos[MAX_MEDICOS];
int n_medicos = 0;

Cita g_citas[MAX_CITAS];
int n_citas = 0;

void menuPrincipal(int rol, char* usuario);
void menuGestionPacientes(int rol);
void menuGestionMedicos(int rol, char* usuario); 
void menuGestionCitas(int rol, char* usuario);
void cargarTodo();
void guardarTodo();

void obtenerCodigoRealDelMedico(char* usuarioLogueado, char* codigoDestino);


int main() {
#ifdef _WIN32
	system("if not exist data mkdir data");
#else
	system("mkdir -p data");
#endif
	
	// 1. Cargar datos
	cargarTodo();
	
	// 2. Login
	char usuarioActual[MAX_STR]; 
	int rolActual = sistemaAutenticacion(usuarioActual);
	
	// 3. Menú
	menuPrincipal(rolActual, usuarioActual);
	
	// 4. Guardar
	guardarTodo();
	
	return 0;
}

void obtenerCodigoRealDelMedico(char* usuarioLogueado, char* codigoDestino) {
	strcpy(codigoDestino, usuarioLogueado);
	
	for(int i=0; i < n_medicos; i++) {
		if (g_medicos[i].activo) {
			if (strcasecmp(g_medicos[i].nombre, usuarioLogueado) == 0 ||
				strcasecmp(g_medicos[i].codigo, usuarioLogueado) == 0) {
				strcpy(codigoDestino, g_medicos[i].codigo);
				return;
			}
		}
	}
}

// Menus

void menuPrincipal(int rol, char* usuario) {
	int opcion = -1;
	char titulo[100];
	char rolTexto[50];
	
	if(rol == ROL_ADMIN) strcpy(rolTexto, "ADMINISTRADOR");
	else if(rol == ROL_MEDICO) strcpy(rolTexto, "MEDICO");
	else strcpy(rolTexto, "PACIENTE");
	
	sprintf(titulo, "BIENVENIDO: %s", usuario);
	
	while (opcion != 0) {
		dibujarEncabezado(titulo);
		imprimirTextoCentrado(rolTexto, COLOR_CYAN);
		printf("\n");
		
		if (rol == ROL_ADMIN || rol == ROL_MEDICO)
			imprimirOpcionCentrada("1", "Gestion de Pacientes");
		
		if (rol == ROL_ADMIN)
			imprimirOpcionCentrada("2", "Gestion de Medicos");
		else if (rol == ROL_MEDICO)
			imprimirOpcionCentrada("2", "Mi Perfil y Agenda"); 
		
		imprimirOpcionCentrada("3", "Gestion de Citas");
		
		if (rol == ROL_ADMIN)
			imprimirOpcionCentrada("4", "Reportes Generales");
		
		printf("\n");
		int lenSalida = strlen("[0] Salir y Guardar");
		int padSalida = (60 - lenSalida) / 2;
		for(int i=0; i<padSalida; i++) printf(" ");
		printf(COLOR_ROJO "[0]" COLOR_RESET " Salir y Guardar\n");
		
		imprimirLinea(60);
		opcion = leerEntero("   > Seleccione opcion: ");
		
		switch (opcion) {
		case 1:
			if (rol == ROL_PACIENTE) { imprimirError("Acceso Denegado"); pausar(); }
			else menuGestionPacientes(rol);
			break;
		case 2:
			if (rol == ROL_PACIENTE) { imprimirError("Acceso Denegado"); pausar(); }
			else menuGestionMedicos(rol, usuario); 
			break;
		case 3: 
			menuGestionCitas(rol, usuario); 
			break;
		case 4:
			if (rol == ROL_ADMIN) {
				menuReportes(g_citas, n_citas, 
							 g_pacientes, n_pacientes, 
							 g_medicos, n_medicos);
			}
			else { imprimirError("Acceso Denegado"); pausar(); }
			break;
		case 0: 
			barraCarga("Guardando y saliendo"); 
			break;
		default: 
			imprimirError("Opcion invalida"); 
			pausar();
		}
	}
}

void menuGestionPacientes(int rol) {
	int op = -1;
	while(op != 0) {
		dibujarEncabezado("GESTION DE PACIENTES");
		printf(COLOR_VERDE "   [1]" COLOR_RESET " Listar Pacientes\n");
		printf(COLOR_VERDE "   [2]" COLOR_RESET " Registrar Nuevo Paciente\n");
		printf(COLOR_ROJO  "   [0]" COLOR_RESET " Volver\n");
		imprimirLinea(60);
		op = leerEntero("   > ");
		
		if (op == 1) listarPacientes(g_pacientes, n_pacientes);
		else if (op == 2) registrarPaciente(g_pacientes, &n_pacientes);
		else if (op != 0) { imprimirError("Invalido"); pausar(); }
	}
}

void menuGestionMedicos(int rol, char* usuario) {
	int op = -1;
	
	char codigoReal[50];
	if (rol == ROL_MEDICO) {
		obtenerCodigoRealDelMedico(usuario, codigoReal);
	}
	
	while(op != 0) {
		if (rol == ROL_ADMIN) {
			dibujarEncabezado("GESTION DE MEDICOS ");
			printf(COLOR_VERDE "   [1]" COLOR_RESET " Listar todos los medicos\n");
			printf(COLOR_VERDE "   [2]" COLOR_RESET " Registrar nuevo medico\n");
			printf(COLOR_VERDE "   [3]" COLOR_RESET " Modificar horario de medico\n");
		} else {
			dibujarEncabezado("MI PERFIL ");
			printf(COLOR_CYAN  "   Medico: " NEGRITA "%s\n" COLOR_RESET, codigoReal);
			printf("\n");
			printf(COLOR_VERDE "   [1]" COLOR_RESET " Ver Agenda de Citas\n");
			printf(COLOR_VERDE "   [3]" COLOR_RESET " Modificar Horario\n");
		}
		printf(COLOR_ROJO  "   [0]" COLOR_RESET " Volver\n");
		imprimirLinea(60);
		op = leerEntero("   > ");
		
		if (op == 1) {
			if (rol == ROL_ADMIN) listarMedicos(g_medicos, n_medicos);
			else verCitasDeMedico(g_citas, n_citas, codigoReal); 
		}
		else if (op == 2 && rol == ROL_ADMIN) {
			registrarMedico(g_medicos, &n_medicos);
		}
		else if (op == 3) {
			if (rol == ROL_MEDICO) {
				printf(COLOR_AMARILLO ">> Modificando horario para: %s\n" COLOR_RESET, codigoReal);
				modificarHorarioMedico(g_medicos, n_medicos); 
			} else {
				modificarHorarioMedico(g_medicos, n_medicos);
			}
		}
		else if (op != 0) { imprimirError("Invalido"); pausar(); }
	}
}

void menuGestionCitas(int rol, char* usuario) {
	int op = -1;
	
	char codigoReal[50];
	obtenerCodigoRealDelMedico(usuario, codigoReal);
	
	while(op != 0) {
		dibujarEncabezado("GESTION DE CITAS");
		printf(COLOR_VERDE "   [1]" COLOR_RESET " Agendar Nueva Cita\n");
		
		if (rol == ROL_MEDICO) 
			printf(COLOR_VERDE "   [2]" COLOR_RESET " Ver MI Agenda\n");
		else 
			printf(COLOR_VERDE "   [2]" COLOR_RESET " Ver Historial General\n");
		
		// NUEVA OPCIÓN
		printf(COLOR_AMARILLO "   [3]" COLOR_RESET " Reagendar Cita (Cambiar Fecha/Hora)\n");
		
		if (rol == ROL_PACIENTE || rol == ROL_ADMIN)
			printf(COLOR_AMARILLO "   [4]" COLOR_RESET " Cancelar Cita\n");
		
		if (rol == ROL_MEDICO)
			printf(COLOR_ROJO     "   [5]" COLOR_RESET " Reportar Ausencia\n");
		
		printf(COLOR_ROJO  "   [0]" COLOR_RESET " Volver\n");
		imprimirLinea(60);
		op = leerEntero("   > ");
		
		if (op == 1) {
			agendarCita(g_citas, &n_citas, g_pacientes, n_pacientes, g_medicos, n_medicos, rol, usuario);
		}
		else if (op == 2) {
			if (rol == ROL_MEDICO) verCitasDeMedico(g_citas, n_citas, codigoReal);
			else listarCitas(g_citas, n_citas);
		}
		else if (op == 3) {
			// Llamada a la nueva función de reagendar
			reagendarCita(g_citas, n_citas, g_medicos, n_medicos, rol, usuario);
		}
		else if (op == 4 && (rol == ROL_PACIENTE || rol == ROL_ADMIN)) {
			cancelarCita(g_citas, n_citas, rol, usuario);
		}
		else if (op == 5 && rol == ROL_MEDICO) {
			registrarAusencia(g_citas, n_citas, rol, codigoReal);
		}
		else if (op != 0) { imprimirError("Invalido"); pausar(); }
	}
}

void cargarTodo() {
	cargarPacientes(g_pacientes, &n_pacientes);
	cargarMedicos(g_medicos, &n_medicos);
	cargarCitas(g_citas, &n_citas);
}

void guardarTodo() {
	guardarPacientes(g_pacientes, n_pacientes);
	guardarMedicos(g_medicos, n_medicos);
	guardarCitas(g_citas, n_citas);
}
