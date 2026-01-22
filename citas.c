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
			return 0; 
		}
	}
	return 1; 
}

//  MENÚ DE FECHAS
void seleccionarFechaMenu(char* fechaSeleccionada, int diasVista) {
	time_t t = time(NULL);
	struct tm fechaActual = *localtime(&t);
	
	char opciones[30][11]; // Guardaremos las fechas string aquí
	
	printf("\n");
	imprimirTextoCentrado("SELECCIONE UNA FECHA", NEGRITA);
	printf(COLOR_CYAN "   ----------------------------------------\n" COLOR_RESET);
	
	for(int i = 0; i < diasVista; i++) {
		// Formatear fecha actual a string DD/MM/AAAA
		strftime(opciones[i], 11, "%d/%m/%Y", &fechaActual);
		printf(COLOR_VERDE "   [%d]" COLOR_RESET " %s\n", i + 1, opciones[i]);
		
		// Sumar 1 día para la siguiente iteración
		fechaActual.tm_mday += 1;
		mktime(&fechaActual); // Normalizar estructura
	}
	printf(COLOR_CYAN "   ----------------------------------------\n" COLOR_RESET);
	
	int op = -1;
	do {
		op = leerEntero("   > Elija el numero de la fecha: ");
		if(op < 1 || op > diasVista) imprimirError("Opcion invalida.");
	} while(op < 1 || op > diasVista);
	
	strcpy(fechaSeleccionada, opciones[op-1]);
}

// --- MENÚ AUTOMÁTICO DE HORAS (SLOTS) ---
int seleccionarHoraMenu(const Cita citas[], int n_citas, char* codMedico, char* fecha, char* inicio, char* fin, char* horaSeleccionada) {
	int hInicio, mInicio, hFin, mFin;
	// Parsear horario del médico (ej: "08:00" -> 8, 0)
	sscanf(inicio, "%d:%d", &hInicio, &mInicio);
	sscanf(fin, "%d:%d", &hFin, &mFin);
	
	// Convertir todo a minutos desde medianoche
	int minActual = hInicio * 60 + mInicio;
	int minFin = hFin * 60 + mFin;
	
	char slots[50][6]; 
	int estados[50];   // 1=Libre, 0=Ocupado
	int count = 0;
	
	printf("\n");
	imprimirTextoCentrado("HORARIOS DISPONIBLES", NEGRITA);
	printf(COLOR_CYAN "   ----------------------------------------\n" COLOR_RESET);
	
	while(minActual < minFin) {
		// Reconvertir minutos a HH:MM
		int h = minActual / 60;
		int m = minActual % 60;
		sprintf(slots[count], "%02d:%02d", h, m);
		
		// Verificar disponibilidad en tiempo real
		if(verificarDisponibilidad(citas, n_citas, codMedico, fecha, slots[count])) {
			estados[count] = 1; // Libre
			printf(COLOR_VERDE "   [%d] %s" COLOR_RESET "\n", count + 1, slots[count]);
		} else {
			estados[count] = 0; // Ocupado
			printf(COLOR_ROJO  "   [X] %s (Ocupado)" COLOR_RESET "\n", slots[count]);
		}
		
		count++;
		minActual += DURACION_CITA; // Avanzar 30 mins
		if(count >= 50) break; // Seguridad
	}
	printf(COLOR_CYAN "   ----------------------------------------\n" COLOR_RESET);
	
	if(count == 0) {
		imprimirError("Este medico no tiene horario configurado o disponible.");
		return 0;
	}
	
	int op = -1;
	int valido = 0;
	do {
		op = leerEntero("   > Elija el numero de la hora: ");
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

// Filtro de especialidades dinámico
int seleccionarEspecialidad(const Medico medicos[], int n_medicos, char* espSeleccionada) {
	char listaUnica[50][50];
	int cantidadUnica = 0;
	
	// Extraer especialidades únicas
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

// ==========================================
// 2. LÓGICA PRINCIPAL: AGENDAR CITA
// ==========================================

void agendarCita(Cita citas[], int *n_citas, 
				 const Paciente pacientes[], int n_pacientes,
				 const Medico medicos[], int n_medicos,
				 int rol, char* usuarioActual) {
	
	if (*n_citas >= MAX_CITAS) {
		imprimirError("Base de datos de citas llena.");
		pausar(); return;
	}
	
	// Validación inicial
	int medicosActivos = 0;
	for(int i=0; i<n_medicos; i++) if(medicos[i].activo) medicosActivos++;
	if(medicosActivos == 0) {
		imprimirError("No hay medicos registrados en el sistema.");
		pausar(); return;
	}
	
	Cita nueva;
	nueva.id = (*n_citas) + 1;
	nueva.estado = ESTADO_ACTIVA;
	
	dibujarEncabezado("AGENDAR NUEVA CITA");
	
	// --- PASO 1: IDENTIFICAR PACIENTE ---
	if (rol == 3) { // Paciente
		strcpy(nueva.cedula_paciente, usuarioActual);
		printf(COLOR_CYAN "   Paciente: " COLOR_RESET "%s (Usted)\n", usuarioActual);
	} else {
		int idxPac = -1;
		do {
			printf("   Ingrese Cedula del Paciente: ");
			leerCadena(nueva.cedula_paciente, 11);
			
			idxPac = buscarPacientePorCedula(pacientes, n_pacientes, nueva.cedula_paciente);
			
			if (idxPac == -1) {
				imprimirError("Paciente no encontrado. Registrelo primero.");
				char salir; printf("   >> Desea cancelar? (s/n): "); scanf(" %c", &salir); getchar();
				if(salir == 's' || salir == 'S') return;
			}
		} while (idxPac == -1);
		printf(COLOR_VERDE "   >> Paciente identificado: %s\n" COLOR_RESET, pacientes[idxPac].nombre);
	}
	
	// --- PASO 2: SELECCIONAR ESPECIALIDAD ---
	char especialidadFiltro[50];
	if (!seleccionarEspecialidad(medicos, n_medicos, especialidadFiltro)) {
		imprimirError("Seleccion invalida o sin especialidades.");
		pausar(); return;
	}
	printf(COLOR_AMARILLO "\n   >> Filtro: %s\n" COLOR_RESET, especialidadFiltro);
	
	// --- PASO 3: SELECCIONAR MÉDICO ---
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
		imprimirError("No hay medicos disponibles en esa especialidad.");
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
		if(idxMed == -1) imprimirError("Codigo incorrecto o no pertenece a la especialidad.");
	} while(idxMed == -1);
	
	printf(COLOR_VERDE "   >> Seleccionado: %s\n" COLOR_RESET, medicos[idxMed].nombre);
	
	// --- PASO 4: SELECCIONAR FECHA (MENÚ AUTOMÁTICO) ---
	seleccionarFechaMenu(nueva.fecha, 14); // Muestra 14 días
	printf(COLOR_VERDE "   >> Fecha: %s\n" COLOR_RESET, nueva.fecha);
	
	// --- PASO 5: SELECCIONAR HORA (MENÚ AUTOMÁTICO) ---
	if(!seleccionarHoraMenu(citas, *n_citas, nueva.codigo_medico, nueva.fecha, 
							medicos[idxMed].horario_inicio, medicos[idxMed].horario_fin, 
							nueva.hora)) {
		return; 
	}
	
	// --- PASO 6: CONFIRMACIÓN ESTRICTA (s/n) ---
	printf("\n");
	imprimirLinea(60);
	printf(NEGRITA "   RESUMEN DE LA CITA:\n" COLOR_RESET);
	printf("   Paciente: %s\n", nueva.cedula_paciente);
	printf("   Medico:   Dr/a. %s (%s)\n", medicos[idxMed].nombre, medicos[idxMed].especialidad);
	printf("   Fecha:    %s\n", nueva.fecha);
	printf("   Hora:     %s\n", nueva.hora);
	imprimirLinea(60);
	
	char confirma;
	int opcionValida = 0;
	
	do {
		printf(NEGRITA "   > ¿Desea confirmar y agendar esta cita? (s/n): " COLOR_RESET);
		fflush(stdin);
		scanf(" %c", &confirma); 
		while(getchar() != '\n'); // Limpiar buffer de entrada
		
		confirma = tolower(confirma);
		
		if (confirma == 's' || confirma == 'n') {
			opcionValida = 1;
		} else {
			imprimirError("Por favor ingrese 's' para SI o 'n' para NO.");
		}
	} while (!opcionValida);
	
	if (confirma == 's') {
		citas[*n_citas] = nueva;
		(*n_citas)++;
		barraCarga("Agendando cita");
		imprimirExito("¡Cita agendada correctamente!");
	} else {
		printf(COLOR_ROJO "\n   >> Operacion cancelada por el usuario.\n" COLOR_RESET);
	}
	
	pausar();
}
				
				 // 3. REAGENDAR CITA
				 void reagendarCita(Cita citas[], int n_citas, const Medico medicos[], int n_medicos, int rol, char* usuarioActual) {
					 dibujarEncabezado("REAGENDAR CITA");
					 
					 // 1. Mostrar citas del usuario
					 if (rol == 3) { // Paciente
						 printf("   Sus citas activas:\n");
						 int count = 0;
						 for(int i=0; i<n_citas; i++) {
							 if(citas[i].estado == ESTADO_ACTIVA && strcmp(citas[i].cedula_paciente, usuarioActual) == 0) {
								 printf("   [ID: %d] %s %s - Dr. %s\n", citas[i].id, citas[i].fecha, citas[i].hora, citas[i].codigo_medico);
								 count++;
							 }
						 }
						 if(count == 0) { imprimirError("No tiene citas activas para reagendar."); pausar(); return; }
					 } else {
						 listarCitas(citas, n_citas); // Admin/Médico
					 }
					 
					 imprimirLinea(60);
					 int id = leerEntero("   > Ingrese ID de la cita a modificar (0 salir): ");
					 if (id == 0) return;
					 
					 // 2. Buscar cita
					 int idxCita = -1;
					 for (int i = 0; i < n_citas; i++) { if (citas[i].id == id) { idxCita = i; break; } }
					 
					 if (idxCita == -1) { imprimirError("Cita no encontrada."); pausar(); return; }
					 
					 // 3. Validar permisos
					 if (rol == 3 && strcmp(citas[idxCita].cedula_paciente, usuarioActual) != 0) {
						 imprimirError("No tiene permiso para modificar esta cita."); pausar(); return;
					 }
					 if (citas[idxCita].estado != ESTADO_ACTIVA) {
						 imprimirError("Solo se pueden reagendar citas ACTIVAS."); pausar(); return;
					 }
					 
					 // 4. Buscar médico original
					 int idxMed = -1;
					 for (int i = 0; i < n_medicos; i++) {
						 if (strcmp(medicos[i].codigo, citas[idxCita].codigo_medico) == 0) {
							 idxMed = i; break;
						 }
					 }
					 
					 if (idxMed == -1) { imprimirError("El medico no existe."); pausar(); return; }
					 
					 printf(COLOR_CYAN "\n   Actual: %s %s (Dr. %s)\n" COLOR_RESET, 
							citas[idxCita].fecha, citas[idxCita].hora, medicos[idxMed].nombre);
					 
					 printf(NEGRITA "   SELECCIONE NUEVOS DATOS:\n" COLOR_RESET);
					 
					 // 5. Seleccionar Nuevos Datos
					 char nuevaFecha[11], nuevaHora[6];
					 seleccionarFechaMenu(nuevaFecha, 14); 
					 
					 if(!seleccionarHoraMenu(citas, n_citas, citas[idxCita].codigo_medico, nuevaFecha, 
											 medicos[idxMed].horario_inicio, medicos[idxMed].horario_fin, 
											 nuevaHora)) {
						 return; 
					 }
					 
					 // 6. Confirmación 
					 printf("\n");
					 imprimirLinea(60);
					 printf(NEGRITA "   CONFIRMAR CAMBIO:\n" COLOR_RESET);
					 printf("   Anterior: %s - %s\n", citas[idxCita].fecha, citas[idxCita].hora);
					 printf(COLOR_VERDE "   Nueva:    %s - %s\n" COLOR_RESET, nuevaFecha, nuevaHora);
					 imprimirLinea(60);
					 
					 char conf;
					 int opValida = 0;
					 
					 do {
						 printf("   > ¿Confirmar reagendamiento? (s/n): ");
						 fflush(stdin);
						 scanf(" %c", &conf); 
						 while(getchar() != '\n'); 
						 
						 conf = tolower(conf);
						 
						 if (conf == 's' || conf == 'n') {
							 opValida = 1;
						 } else {
							 imprimirError("Ingrese 's' o 'n'.");
						 }
					 } while (!opValida);
					 
					 if (conf == 's') {
						 strcpy(citas[idxCita].fecha, nuevaFecha);
						 strcpy(citas[idxCita].hora, nuevaHora);
						 barraCarga("Actualizando agenda");
						 imprimirExito("Cita reagendada exitosamente.");
					 } else {
						 printf(COLOR_ROJO "   >> Cambio cancelado.\n" COLOR_RESET);
					 }
					 pausar();
				 }
				 
				 // 4.CANCELAR, AUSENCIA
				 
				 void cancelarCita(Cita citas[], int n_citas, int rol, char* usuarioActual) {
					 dibujarEncabezado("CANCELAR CITA");
					 
					 if (rol == 3) { // Paciente: Solo ve SUS citas
						 printf("   Sus citas activas:\n");
						 int count = 0;
						 for(int i=0; i<n_citas; i++) {
							 if(citas[i].estado == ESTADO_ACTIVA && strcmp(citas[i].cedula_paciente, usuarioActual) == 0) {
								 printf("   [ID: %d] %s - %s (Dr. %s)\n", citas[i].id, citas[i].fecha, citas[i].hora, citas[i].codigo_medico);
								 count++;
							 }
						 }
						 if(count == 0) { printf("   No tiene citas activas para cancelar.\n"); pausar(); return; }
					 } else {
						 listarCitas(citas, n_citas); // Admin ve todo
					 }
					 
					 imprimirLinea(60);
					 int id = leerEntero("   > Ingrese ID de cita a cancelar (0 salir): ");
					 if (id == 0) return;
					 
					 int idx = -1;
					 for (int i = 0; i < n_citas; i++) {
						 if (citas[i].id == id) {
							 idx = i;
							 break;
						 }
					 }
					 
					 if (idx == -1) { imprimirError("Cita no encontrada."); pausar(); return; }
					 
					 if (rol == 3 && strcmp(citas[idx].cedula_paciente, usuarioActual) != 0) {
						 imprimirError("Esa cita no es suya.");
						 pausar();
						 return;
					 }
					 
					 if (citas[idx].estado != ESTADO_ACTIVA) {
						 imprimirError("La cita ya no esta activa.");
						 pausar();
						 return;
					 }
					 
					 citas[idx].estado = ESTADO_CANCELADA;
					 imprimirExito("Cita cancelada.");
					 pausar();
				 }
				 
				 void registrarAusencia(Cita citas[], int n_citas, int rol, char* usuarioActual) {
					 dibujarEncabezado("REGISTRAR AUSENCIA");
					 verCitasDeMedico(citas, n_citas, usuarioActual);
					 int id = leerEntero("   > ID de la cita donde paciente FALTO: ");
					 
					 int idx = -1;
					 for (int i = 0; i < n_citas; i++) {
						 if (citas[i].id == id) { idx = i; break; }
					 }
					 
					 if (idx == -1) { imprimirError("No existe ese ID."); pausar(); return; }
					 
					 if (strcmp(citas[idx].codigo_medico, usuarioActual) != 0 && rol != 1) {
						 imprimirError("Esa cita no es de su agenda.");
						 pausar(); return;
					 }
					 
					 if (citas[idx].estado != ESTADO_ACTIVA) {
						 imprimirError("La cita no esta activa.");
						 pausar(); return;
					 }
					 
					 citas[idx].estado = ESTADO_AUSENTE;
					 imprimirExito("Paciente marcado como AUSENTE.");
					 pausar();
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
				
				 // 5. PERSISTENCIA
				 
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
