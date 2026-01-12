#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h> 
#include "citas.h"
#include "utils.h"

#ifdef _WIN32
#define strcasecmp _stricmp
#endif

#define DURACION_CITA 30 

int verificarDisponibilidad(const Cita citas[], int n_citas, char* codMedico, char* fecha, char* hora) {
	for (int i = 0; i < n_citas; i++) {
		if (citas[i].estado == ESTADO_ACTIVA &&
			strcmp(citas[i].codigo_medico, codMedico) == 0 &&
			strcmp(citas[i].fecha, fecha) == 0 &&
			strcmp(citas[i].hora, hora) == 0) {
			return 0; // Ocupado
		}
	}
	return 1; // Disponible
}

// Menu fechas
void seleccionarFechaMenu(char* fechaSeleccionada, int diasVista) {
	time_t t = time(NULL);
	struct tm fechaActual = *localtime(&t);
	
	char opciones[30][11]; 
	
	printf("\n");
	imprimirTextoCentrado("SELECCIONE UNA FECHA", NEGRITA);
	printf(COLOR_CYAN "   ----------------------------------------\n" COLOR_RESET);
	
	for(int i = 0; i < diasVista; i++) {
		strftime(opciones[i], 11, "%d/%m/%Y", &fechaActual);
		printf(COLOR_VERDE "   [%d]" COLOR_RESET " %s\n", i + 1, opciones[i]);
		
		fechaActual.tm_mday += 1;
		mktime(&fechaActual); 
	}
	printf(COLOR_CYAN "   ----------------------------------------\n" COLOR_RESET);
	
	int op = -1;
	do {
		op = leerEntero("   > Elija la fecha: ");
		if(op < 1 || op > diasVista) imprimirError("Opcion invalida.");
	} while(op < 1 || op > diasVista);
	
	strcpy(fechaSeleccionada, opciones[op-1]);
}

// Menu horas
int seleccionarHoraMenu(const Cita citas[], int n_citas, char* codMedico, char* fecha, char* inicio, char* fin, char* horaSeleccionada) {
	int hInicio, mInicio, hFin, mFin;
	sscanf(inicio, "%d:%d", &hInicio, &mInicio);
	sscanf(fin, "%d:%d", &hFin, &mFin);
	
	int minActual = hInicio * 60 + mInicio;
	int minFin = hFin * 60 + mFin;
	
	char slots[50][6]; 
	int estados[50];  
	int count = 0;
	
	printf("\n");
	imprimirTextoCentrado("HORARIOS DISPONIBLES", NEGRITA);
	printf(COLOR_CYAN "   ----------------------------------------\n" COLOR_RESET);
	
	while(minActual < minFin) {
		int h = minActual / 60;
		int m = minActual % 60;
		sprintf(slots[count], "%02d:%02d", h, m);
		
		if(verificarDisponibilidad(citas, n_citas, codMedico, fecha, slots[count])) {
			estados[count] = 1; 
			printf(COLOR_VERDE "   [%d] %s" COLOR_RESET "\n", count + 1, slots[count]);
		} else {
			estados[count] = 0; 
			printf(COLOR_ROJO  "   [X] %s (Ocupado)" COLOR_RESET "\n", slots[count]);
		}
		
		count++;
		minActual += DURACION_CITA; 
		if(count >= 50) break; 
	}
	printf(COLOR_CYAN "   ----------------------------------------\n" COLOR_RESET);
	
	if(count == 0) {
		imprimirError("Este medico no tiene horario configurado.");
		return 0;
	}
	
	int op = -1;
	int valido = 0;
	do {
		op = leerEntero("   > Elija la hora: ");
		if(op < 1 || op > count) {
			imprimirError("Opcion invalida.");
		} else if (estados[op-1] == 0) {
			imprimirError("Ese horario esta OCUPADO. Elija otro.");
		} else {
			valido = 1;
		}
	} while(!valido);
	
	strcpy(horaSeleccionada, slots[op-1]);
	return 1;
}

// Filtro de especialidades
int seleccionarEspecialidad(const Medico medicos[], int n_medicos, char* espSeleccionada) {
	char listaUnica[50][50];
	int cantidadUnica = 0;
	
	for (int i = 0; i < n_medicos; i++) {
		if (!medicos[i].activo) continue;
		int yaExiste = 0;
		for (int j = 0; j < cantidadUnica; j++) {
			if (strcasecmp(medicos[i].especialidad, listaUnica[j]) == 0) {
				yaExiste = 1; break;
			}
		}
		if (!yaExiste) {
			strcpy(listaUnica[cantidadUnica], medicos[i].especialidad);
			cantidadUnica++;
		}
	}
	
	if (cantidadUnica == 0) return 0;
	
	printf("\n");
	imprimirTextoCentrado("FILTRAR POR ESPECIALIDAD", NEGRITA);
	printf(COLOR_CYAN "   ----------------------------------------\n" COLOR_RESET);
	for (int i = 0; i < cantidadUnica; i++) {
		printf(COLOR_VERDE "   [%d]" COLOR_RESET " %s\n", i + 1, listaUnica[i]);
	}
	printf(COLOR_CYAN "   ----------------------------------------\n" COLOR_RESET);
	
	int op = leerEntero("   > Elija el numero de la especialidad: ");
	if (op < 1 || op > cantidadUnica) return 0;
	
	strcpy(espSeleccionada, listaUnica[op - 1]);
	return 1;
}

void verCitasDeMedico(const Cita citas[], int n_citas, char* codigoMedico) {
	printf("\n");
	dibujarEncabezado("MI AGENDA DE CITAS");
	printf("   Medico: " NEGRITA "%s" COLOR_RESET "\n", codigoMedico);
	
	printf(COLOR_CYAN "   ===============================================================\n" COLOR_RESET);
	printf(NEGRITA "   %-4s | %-11s | %-6s | %-11s | %-12s\n" COLOR_RESET, 
		   "ID", "FECHA", "HORA", "PACIENTE", "ESTADO");
	printf(COLOR_CYAN "   ===============================================================\n" COLOR_RESET);
	
	int count = 0;
	for(int i=0; i<n_citas; i++) {
		if (strcmp(citas[i].codigo_medico, codigoMedico) == 0) {
			char estadoStr[15], color[10];
			
			switch(citas[i].estado) {
			case ESTADO_ACTIVA:    strcpy(estadoStr, "ACTIVA"); strcpy(color, COLOR_VERDE); break;
			case ESTADO_CANCELADA: strcpy(estadoStr, "CANCELADA"); strcpy(color, COLOR_ROJO); break;
			case ESTADO_AUSENTE:   strcpy(estadoStr, "AUSENTE"); strcpy(color, COLOR_AMARILLO); break;
			default:               strcpy(estadoStr, "REALIZADA"); strcpy(color, COLOR_AZUL); break;
			}
			
			printf("   %-4d | %-11s | %-6s | %-11s | %s%s" COLOR_RESET "\n", 
				   citas[i].id, citas[i].fecha, citas[i].hora, citas[i].cedula_paciente, color, estadoStr);
			count++;
		}
	}
	if (count == 0) printf(COLOR_AMARILLO "   (No tiene citas asignadas)\n" COLOR_RESET);
	printf(COLOR_CYAN "   ===============================================================\n" COLOR_RESET);
	pausar();
}

void agendarCita(Cita citas[], int *n_citas, 
				 const Paciente pacientes[], int n_pacientes,
				 const Medico medicos[], int n_medicos,
				 int rol, char* usuarioActual) {
	
	if (*n_citas >= MAX_CITAS) {
		imprimirError("Base de datos de citas llena.");
		pausar(); return;
	}
	
	int medicosActivos = 0;
	for(int i=0; i<n_medicos; i++) if(medicos[i].activo) medicosActivos++;
	if(medicosActivos == 0) {
		imprimirError("No hay medicos registrados.");
		pausar(); return;
	}
	
	Cita nueva;
	nueva.id = (*n_citas) + 1;
	nueva.estado = ESTADO_ACTIVA;
	
	dibujarEncabezado("AGENDAR NUEVA CITA");
	
	// 1. Paciente
	if (rol == 3) { 
		strcpy(nueva.cedula_paciente, usuarioActual);
		printf(COLOR_CYAN "   Paciente: " COLOR_RESET "%s (Usted)\n", usuarioActual);
	} else {
		int idxPac = -1;
		do {
			printf("   Ingrese cedula del Paciente: ");
			leerCadena(nueva.cedula_paciente, 11);
			idxPac = buscarPacientePorCedula(pacientes, n_pacientes, nueva.cedula_paciente);
			if (idxPac == -1) {
				imprimirError("Paciente no encontrado.");
				char salir; printf("   >> Cancelar? (s/n): "); scanf(" %c", &salir); getchar();
				if(salir == 's' || salir == 'S') return;
			}
		} while (idxPac == -1);
		printf(COLOR_VERDE "   >> Paciente: %s\n" COLOR_RESET, pacientes[idxPac].nombre);
	}
	
	// 2. Especialidad
	char especialidadFiltro[50];
	if (!seleccionarEspecialidad(medicos, n_medicos, especialidadFiltro)) {
		imprimirError("Seleccion invalida.");
		pausar(); return;
	}
	printf(COLOR_AMARILLO "\n   >> Filtro: %s\n" COLOR_RESET, especialidadFiltro);
	
	// 3. Medico
	printf("\n");
	imprimirLinea(60);
	printf(NEGRITA "   MEDICOS DE %s:\n" COLOR_RESET, especialidadFiltro);
	printf("   %-10s | %-20s | %s\n", "CODIGO", "NOMBRE", "HORARIO");
	imprimirLinea(60);
	
	int encontrados = 0;
	for(int i=0; i<n_medicos; i++) {
		if(medicos[i].activo && strcasecmp(medicos[i].especialidad, especialidadFiltro) == 0) {
			printf("   %-10s | %-20s | %s-%s\n", 
				   medicos[i].codigo, medicos[i].nombre,
				   medicos[i].horario_inicio, medicos[i].horario_fin);
			encontrados++;
		}
	}
	
	if (encontrados == 0) {
		imprimirError("No hay medicos en esta especialidad.");
		pausar(); return;
	}
	
	int idxMed = -1;
	do {
		printf("\n   Ingrese CODIGO del medico: ");
		leerCadena(nueva.codigo_medico, 10);
		for(int i=0; i<n_medicos; i++) {
			if(strcmp(medicos[i].codigo, nueva.codigo_medico) == 0 && 
			   medicos[i].activo &&
			   strcasecmp(medicos[i].especialidad, especialidadFiltro) == 0) {
				idxMed = i; break;
			}
		}
		if(idxMed == -1) imprimirError("Codigo incorrecto.");
	} while(idxMed == -1);
	
	printf(COLOR_VERDE "   >> Medico: %s\n" COLOR_RESET, medicos[idxMed].nombre);
	
	// 4. Fecha
	seleccionarFechaMenu(nueva.fecha, 14); // Muestra próximos 14 días
	printf(COLOR_VERDE "   >> Fecha: %s\n" COLOR_RESET, nueva.fecha);
	
	// 5. Hora
	if(!seleccionarHoraMenu(citas, *n_citas, nueva.codigo_medico, nueva.fecha, 
							medicos[idxMed].horario_inicio, medicos[idxMed].horario_fin, 
							nueva.hora)) {
		return; 
	}
	
	citas[*n_citas] = nueva;
	(*n_citas)++;
	imprimirExito("Cita agendada correctamente.");
	pausar();
}
				 
				 
				 void cancelarCita(Cita citas[], int n_citas, int rol, char* usuarioActual) {
					 dibujarEncabezado("CANCELAR CITA");
					 if (rol == 3) {
						 printf("   Sus citas activas:\n");
						 int count = 0;
						 for(int i=0; i<n_citas; i++) {
							 if(citas[i].estado == ESTADO_ACTIVA && strcmp(citas[i].cedula_paciente, usuarioActual) == 0) {
								 printf("   [ID: %d] %s - %s (Dr. %s)\n", citas[i].id, citas[i].fecha, citas[i].hora, citas[i].codigo_medico);
								 count++;
							 }
						 }
						 if(count == 0) { printf("   No tiene citas activas.\n"); pausar(); return; }
					 } else {
						 listarCitas(citas, n_citas);
					 }
					 
					 imprimirLinea(60);
					 int id = leerEntero("   > ID a cancelar (0 salir): ");
					 if (id == 0) return;
					 
					 int idx = -1;
					 for (int i = 0; i < n_citas; i++) { if (citas[i].id == id) { idx = i; break; } }
					 
					 if (idx == -1) { imprimirError("Cita no encontrada."); pausar(); return; }
					 if (rol == 3 && strcmp(citas[idx].cedula_paciente, usuarioActual) != 0) { imprimirError("No es su cita."); pausar(); return; }
					 if (citas[idx].estado != ESTADO_ACTIVA) { imprimirError("Ya no esta activa."); pausar(); return; }
					 
					 citas[idx].estado = ESTADO_CANCELADA;
					 imprimirExito("Cancelada.");
					 pausar();
				 }
				 
				 void registrarAusencia(Cita citas[], int n_citas, int rol, char* usuarioActual) {
					 dibujarEncabezado("REGISTRAR AUSENCIA");
					 verCitasDeMedico(citas, n_citas, usuarioActual);
					 int id = leerEntero("   > ID cita faltante: ");
					 int idx = -1;
					 for (int i = 0; i < n_citas; i++) { if (citas[i].id == id) { idx = i; break; } }
					 
					 if (idx == -1) { imprimirError("No existe."); pausar(); return; }
					 if (strcmp(citas[idx].codigo_medico, usuarioActual) != 0 && rol != 1) { imprimirError("No es su cita."); pausar(); return; }
					 if (citas[idx].estado != ESTADO_ACTIVA) { imprimirError("Cita no activa."); pausar(); return; }
					 
					 citas[idx].estado = ESTADO_AUSENTE;
					 imprimirExito("Marcado como Ausente.");
					 pausar();
				 }
				 
				 void listarCitas(const Cita citas[], int n_citas) {
					 printf("\n");
					 printf(COLOR_CYAN "======================================================================\n" COLOR_RESET);
					 printf(NEGRITA "%-4s | %-11s | %-8s | %-11s | %-6s | %-9s\n" COLOR_RESET, "ID", "PACIENTE", "MEDICO", "FECHA", "HORA", "ESTADO");
					 printf(COLOR_CYAN "======================================================================\n" COLOR_RESET);
					 
					 for (int i = 0; i < n_citas; i++) {
						 char est[15], col[10];
						 switch(citas[i].estado) {
						 case ESTADO_ACTIVA:    strcpy(est, "ACTIVA"); strcpy(col, COLOR_VERDE); break;
						 case ESTADO_CANCELADA: strcpy(est, "CANCELADA"); strcpy(col, COLOR_ROJO); break;
						 case ESTADO_AUSENTE:   strcpy(est, "AUSENTE");strcpy(col, COLOR_AMARILLO); break;
						 default:               strcpy(est, "HECHA");  strcpy(col, COLOR_AZUL); break;
						 }
						 printf("%-4d | %-11s | %-8s | %-11s | %-6s | %s%-9s" COLOR_RESET "\n",
								citas[i].id, citas[i].cedula_paciente, citas[i].codigo_medico,
								citas[i].fecha, citas[i].hora, col, est);
					 }
					 printf(COLOR_CYAN "======================================================================\n" COLOR_RESET);
					 pausar();
				 }
				 
				 void cargarCitas(Cita citas[], int *cantidad) {
					 FILE *fp = fopen(ARCHIVO_CITAS, "r");
					 *cantidad = 0;
					 if (!fp) return;
					 char linea[256];
					 while (fgets(linea, sizeof(linea), fp)) {
						 if (*cantidad >= MAX_CITAS) break;
						 Cita c;
						 int leidos = sscanf(linea, "%d;%[^;];%[^;];%[^;];%[^;];%d",
											 &c.id, c.cedula_paciente, c.codigo_medico, 
											 c.fecha, c.hora, &c.estado);
						 if (leidos == 6) { citas[*cantidad] = c; (*cantidad)++; }
					 }
					 fclose(fp);
				 }
				 
				 void guardarCitas(const Cita citas[], int cantidad) {
					 FILE *fp = fopen(ARCHIVO_CITAS, "w");
					 if (!fp) return;
					 for (int i = 0; i < cantidad; i++) {
						 fprintf(fp, "%d;%s;%s;%s;%s;%d\n",
								 citas[i].id, citas[i].cedula_paciente, citas[i].codigo_medico,
								 citas[i].fecha, citas[i].hora, citas[i].estado);
					 }
					 fclose(fp);
				 }
				 
				 void inicializarCitas(Cita citas[], int *cantidad) { *cantidad = 0; }
