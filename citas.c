/* Archivo: citas.c */
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

// ==========================================
// 1. FUNCIONES AUXILIARES DE SEGURIDAD
// ==========================================

// Limpiador para arreglar bugs de saltos de linea invisibles
void limpiarString(char *str) {
	for(int i=0; str[i]; i++) {
		if(str[i] == '\r' || str[i] == '\n' || str[i] == ' ') {
			str[i] = '\0';
			break;
		}
	}
}

void sanitizarTextoCSV(char* cadena) {
	if (!cadena) return;
	for (int i = 0; cadena[i] != '\0'; i++) {
		if (cadena[i] == ';') cadena[i] = ','; 
		if (cadena[i] == '\n' || cadena[i] == '\r') cadena[i] = ' '; 
	}
}

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

void seleccionarFechaMenu(char* fechaSeleccionada, int diasVista) {
	time_t t = time(NULL);
	struct tm fechaActual = *localtime(&t);
	char opciones[30][11]; 
	
	printf("\n");
	printf(COLOR_CYAN "   ----------------------------------------\n" COLOR_RESET);
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
		op = leerEntero("   > Elija el numero de la fecha: ");
		if(op < 1 || op > diasVista) imprimirError("Opcion invalida.");
	} while(op < 1 || op > diasVista);
	
	strcpy(fechaSeleccionada, opciones[op-1]);
}

int seleccionarHoraMenu(const Cita citas[], int n_citas, char* codMedico, char* fecha, char* inicio, char* fin, char* horaSeleccionada) {
	int hInicio, mInicio, hFin, mFin;
	if (sscanf(inicio, "%d:%d", &hInicio, &mInicio) != 2 || sscanf(fin, "%d:%d", &hFin, &mFin) != 2) {
		imprimirError("El horario del medico esta corrupto. Modifiquelo en su perfil.");
		return 0;
	}
	
	int minActual = hInicio * 60 + mInicio;
	int minFin = hFin * 60 + mFin;
	
	char slots[50][6]; 
	int estados[50];   
	int count = 0;
	
	printf("\n");
	printf(COLOR_CYAN "   ----------------------------------------\n" COLOR_RESET);
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
		imprimirError("Este medico no tiene franjas horarias configuradas.");
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
	printf(COLOR_CYAN "   ----------------------------------------\n" COLOR_RESET);
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

void agendarCita(Cita citas[], int *n_citas, const Paciente pacientes[], int n_pacientes, const Medico medicos[], int n_medicos, int rol, char* usuarioActual) {
	if (*n_citas >= MAX_CITAS) { imprimirError("Base de datos llena."); pausar(); return; }
	
	int medicosActivos = 0;
	for(int i=0; i<n_medicos; i++) if(medicos[i].activo) medicosActivos++;
	if(medicosActivos == 0) { imprimirError("No hay medicos en el sistema."); pausar(); return; }
	
	Cita nueva;
	nueva.id = (*n_citas) + 1;
	nueva.estado = ESTADO_ACTIVA;
	
	nueva.peso = 0.0;
	nueva.frec_cardiaca = 0;
	strcpy(nueva.presion, "N/A");
	strcpy(nueva.sintomas, "N/A");
	strcpy(nueva.diagnostico, "Pendiente");
	
	dibujarEncabezado("AGENDAR NUEVA CITA");
	
	if (rol == 3) { 
		strcpy(nueva.cedula_paciente, usuarioActual);
		printf(COLOR_CYAN "   Paciente: " COLOR_RESET "%s (Usted)\n", usuarioActual);
	} else { 
		int idxPac = -1;
		do {
			printf("   Ingrese Cedula del Paciente: ");
			leerCadena(nueva.cedula_paciente, 15);
			idxPac = buscarPacientePorCedula(pacientes, n_pacientes, nueva.cedula_paciente);
			if (idxPac == -1) {
				imprimirError("Paciente no encontrado en el sistema.");
				char salir; printf("   >> Desea cancelar? (s/n): "); scanf(" %c", &salir); getchar();
				if(salir == 's' || salir == 'S') return;
			}
		} while (idxPac == -1);
		printf(COLOR_VERDE "   >> Paciente identificado: %s\n" COLOR_RESET, pacientes[idxPac].nombre);
	}
	
	int idxMed = -1;
	
	if (rol == 2) { 
		// PARCHE: Copiamos y limpiamos basura invisible del usuarioActual
		strcpy(nueva.codigo_medico, usuarioActual);
		limpiarString(nueva.codigo_medico);
		
		for(int i=0; i<n_medicos; i++) {
			char codTmp[20];
			strcpy(codTmp, medicos[i].codigo);
			limpiarString(codTmp); // Limpiamos la lectura del .txt
			
			if(strcmp(codTmp, nueva.codigo_medico) == 0) { 
				idxMed = i; 
				break; 
			}
		}
		if(idxMed == -1) { 
			imprimirError("Error interno: Su usuario no coincide con la lista de medicos.txt"); 
			pausar(); return; 
		}
		printf(COLOR_VERDE "   >> Agendando para su propia agenda (Dr. %s)\n" COLOR_RESET, medicos[idxMed].nombre);
	} else { 
		char especialidadFiltro[50];
		if (!seleccionarEspecialidad(medicos, n_medicos, especialidadFiltro)) {
			imprimirError("Seleccion invalida."); pausar(); return;
		}
		
		printf("\n"); imprimirLinea(70);
		printf(NEGRITA "   MEDICOS DE %s:\n" COLOR_RESET, especialidadFiltro);
		imprimirLinea(70);
		
		int encontrados = 0;
		for(int i=0; i<n_medicos; i++) {
			if(medicos[i].activo && strcasecmp(medicos[i].especialidad, especialidadFiltro) == 0) {
				// MODIFICADO: Ahora muestra el precio en verde al final de la línea
				printf("   %-10s | %-20s | %s-%s | " COLOR_VERDE "$%.2f" COLOR_RESET "\n", 
					   medicos[i].codigo, medicos[i].nombre, medicos[i].horario_inicio, medicos[i].horario_fin, medicos[i].precio_consulta);
				encontrados++;
			}
		}
		if (encontrados == 0) { imprimirError("No hay medicos disponibles."); pausar(); return; }
		
		do {
			printf("\n   Ingrese CODIGO del medico: ");
			leerCadena(nueva.codigo_medico, 10);
			limpiarString(nueva.codigo_medico);
			
			for(int i=0; i<n_medicos; i++) {
				char codTmp[20];
				strcpy(codTmp, medicos[i].codigo);
				limpiarString(codTmp);
				
				if(strcmp(codTmp, nueva.codigo_medico) == 0 && medicos[i].activo && strcasecmp(medicos[i].especialidad, especialidadFiltro) == 0) {
					idxMed = i; break;
				}
			}
			if(idxMed == -1) imprimirError("Codigo incorrecto.");
		} while(idxMed == -1);
	}
	
	seleccionarFechaMenu(nueva.fecha, 14); 
	if(!seleccionarHoraMenu(citas, *n_citas, nueva.codigo_medico, nueva.fecha, medicos[idxMed].horario_inicio, medicos[idxMed].horario_fin, nueva.hora)) return; 
	
	printf("\n"); imprimirLinea(60);
	printf(NEGRITA "   RESUMEN DE LA CITA:\n" COLOR_RESET);
	printf("   Paciente: %s\n", nueva.cedula_paciente);
	printf("   Medico:   Dr/a. %s (%s)\n", medicos[idxMed].nombre, medicos[idxMed].especialidad);
	printf("   Fecha:    %s\n", nueva.fecha);
	printf("   Hora:     %s\n", nueva.hora);
	printf(COLOR_VERDE "   Costo:    $%.2f\n" COLOR_RESET, medicos[idxMed].precio_consulta); // <-- NUEVO
	imprimirLinea(60);
	
	char confirma;
	int opcionValida = 0;
	do {
		printf(NEGRITA "   > ¿Desea confirmar y agendar esta cita? (s/n): " COLOR_RESET);
		fflush(stdin); scanf(" %c", &confirma); while(getchar() != '\n'); 
		confirma = tolower(confirma);
		if (confirma == 's' || confirma == 'n') opcionValida = 1; else imprimirError("Ingrese 's' o 'n'.");
	} while (!opcionValida);
	
	if (confirma == 's') {
		citas[*n_citas] = nueva; (*n_citas)++;
		barraCarga("Agendando cita"); imprimirExito("¡Cita agendada correctamente!");
	} else {
		printf(COLOR_ROJO "\n   >> Operacion cancelada.\n" COLOR_RESET);
	}
	pausar();
}

// ==========================================
// 3. LA ZONA DE ATENCIÓN (FORMULARIO CON VALIDACIÓN)
// ==========================================

void atenderPaciente(Cita citas[], int n_citas, const Paciente pacientes[], int n_pacientes, char* codigoMedico) {
	dibujarEncabezado("ZONA DE CONSULTORIO - EVALUACION CLINICA");
	
	printf(COLOR_CYAN "   PACIENTES EN SALA DE ESPERA:\n" COLOR_RESET);
	imprimirLinea(65);
	int count = 0;
	char medTmp[20];
	strcpy(medTmp, codigoMedico);
	limpiarString(medTmp);
	
	for(int i = 0; i < n_citas; i++) {
		char citaMedTmp[20];
		strcpy(citaMedTmp, citas[i].codigo_medico);
		limpiarString(citaMedTmp);
		
		if(citas[i].estado == ESTADO_ACTIVA && strcmp(citaMedTmp, medTmp) == 0) {
			char nombrePac[50] = "Desconocido";
			int idxP = buscarPacientePorCedula(pacientes, n_pacientes, citas[i].cedula_paciente);
			if(idxP != -1) strcpy(nombrePac, pacientes[idxP].nombre);
			
			printf("   [ID: %d] Fecha: %s | Hora: %s | Pac: %s\n", 
				   citas[i].id, citas[i].fecha, citas[i].hora, nombrePac);
			count++;
		}
	}
	
	if(count == 0) {
		printf(COLOR_AMARILLO "   No tiene pacientes esperando en este momento.\n" COLOR_RESET);
		pausar(); return;
	}
	imprimirLinea(65);
	
	int id = leerEntero("   > Ingrese el ID del paciente a pasar (0 para salir): ");
	if (id == 0) return;
	
	int idxCita = -1;
	for(int i = 0; i < n_citas; i++) {
		char citaMedTmp[20];
		strcpy(citaMedTmp, citas[i].codigo_medico);
		limpiarString(citaMedTmp);
		
		if(citas[i].id == id && citas[i].estado == ESTADO_ACTIVA && strcmp(citaMedTmp, medTmp) == 0) {
			idxCita = i; break;
		}
	}
	
	if(idxCita == -1) { imprimirError("ID invalido o el paciente no esta en su agenda."); pausar(); return; }
	
	int idxPac = buscarPacientePorCedula(pacientes, n_pacientes, citas[idxCita].cedula_paciente);
	
	limpiarPantalla();
	dibujarEncabezado("HISTORIAL CLINICO - FORMULARIO DE INGRESO");
	
	if(idxPac != -1) {
		printf(COLOR_CYAN "   --- DATOS DEL PACIENTE ---\n" COLOR_RESET);
		printf(NEGRITA "   Nombre: " COLOR_RESET "%s\n", pacientes[idxPac].nombre);
		printf(NEGRITA "   C.I:    " COLOR_RESET "%s\n", pacientes[idxPac].cedula);
		printf(NEGRITA "   Edad:   " COLOR_RESET "%d anios\n", pacientes[idxPac].edad);
	}
	
	printf(COLOR_CYAN "\n   --- TRIAGE (SIGNOS VITALES) ---\n" COLOR_RESET);
	char inputBuffer[50];
	
	float pesoTemporal = 0.0;
	do {
		printf("   > Peso del paciente (Ej: 75.5 kg): ");
		leerCadena(inputBuffer, 20);
		pesoTemporal = atof(inputBuffer);
		
		if (pesoTemporal <= 0.0 || pesoTemporal > 500.0) {
			imprimirError("Peso invalido. Ingrese un valor mayor a 0 y menor a 500 kg.");
		}
	} while (pesoTemporal <= 0.0 || pesoTemporal > 500.0);
	citas[idxCita].peso = pesoTemporal;
	
	int frecTemporal = 0;
	do {
		frecTemporal = leerEntero("   > Frecuencia Cardiaca (lpm): ");
		if (frecTemporal <= 20 || frecTemporal > 300) {
			imprimirError("Frecuencia cardiaca irreal. Rango aceptado: 20 - 300 lpm.");
		}
	} while (frecTemporal <= 20 || frecTemporal > 300);
	citas[idxCita].frec_cardiaca = frecTemporal;
	
	int presionValida = 0;
	int sys, dia;
	do {
		printf("   > Presion Arterial (Formato SIS/DIA, Ej: 120/80): ");
		leerCadena(inputBuffer, 15);
		
		if (sscanf(inputBuffer, "%d/%d", &sys, &dia) == 2) {
			if (sys >= 50 && sys <= 250 && dia >= 30 && dia <= 150) {
				presionValida = 1;
				strcpy(citas[idxCita].presion, inputBuffer);
			} else {
				imprimirError("Los valores de presion ingresados estan fuera de los limites clinicos humanos.");
			}
		} else {
			imprimirError("Formato incorrecto. Debe incluir la barra '/' entre los numeros.");
		}
	} while (!presionValida);
	sanitizarTextoCSV(citas[idxCita].presion);
	
	printf(COLOR_CYAN "\n   --- EVALUACION MEDICA ---\n" COLOR_RESET);
	printf("   > Dolencias y Sintomas reportados (Max 900 caracteres):\n   > ");
	leerCadena(citas[idxCita].sintomas, 1000);
	sanitizarTextoCSV(citas[idxCita].sintomas); 
	
	printf("\n   > Diagnostico y Receta Final (Max 900 caracteres):\n   > ");
	leerCadena(citas[idxCita].diagnostico, 1000);
	sanitizarTextoCSV(citas[idxCita].diagnostico);
	
	citas[idxCita].estado = ESTADO_REALIZADA; 
	
	printf("\n");
	barraCarga("Procesando formulario clinico y validando datos");
	imprimirExito("Ficha Clinica guardada exitosamente.");
	pausar();
}

// ==========================================
// 4. VISOR DE FICHA CLÍNICA INDIVIDUAL
// ==========================================

void verDetalleFicha(Cita cita, const Paciente pacientes[], int n_pacientes, const Medico medicos[], int n_medicos) {
	limpiarPantalla();
	dibujarEncabezado("FICHA MEDICA DETALLADA");
	
	char nombrePac[50] = "Desconocido";
	for (int i=0; i<n_pacientes; i++) if (strcmp(pacientes[i].cedula, cita.cedula_paciente)==0) strcpy(nombrePac, pacientes[i].nombre);
	
	char nombreMed[50] = "Desconocido";
	for (int i=0; i<n_medicos; i++) {
		char medTmp[20], citaMedTmp[20];
		strcpy(medTmp, medicos[i].codigo); limpiarString(medTmp);
		strcpy(citaMedTmp, cita.codigo_medico); limpiarString(citaMedTmp);
		if (strcmp(medTmp, citaMedTmp)==0) strcpy(nombreMed, medicos[i].nombre);
	}
	
	printf(COLOR_CYAN "   [ DATOS GENERALES ]\n" COLOR_RESET);
	printf("   ID Cita:  %d\n", cita.id);
	printf("   Fecha:    %s a las %s\n", cita.fecha, cita.hora);
	printf("   Paciente: %s (C.I: %s)\n", nombrePac, cita.cedula_paciente);
	
	// Buscamos el precio del médico para imprimirlo
	float precio = 0.0;
	for(int m=0; m<n_medicos; m++) {
		if(strcmp(medicos[m].codigo, cita.codigo_medico) == 0) precio = medicos[m].precio_consulta;
	}
	printf("   Medico:   Dr/a. %s " COLOR_VERDE "(Tarifa: $%.2f)\n" COLOR_RESET, nombreMed, precio);
	
	char estadoStr[25];
	switch(cita.estado) {
	case ESTADO_ACTIVA: strcpy(estadoStr, COLOR_VERDE "ACTIVA (Agendada)" COLOR_RESET); break;
	case ESTADO_CANCELADA: strcpy(estadoStr, COLOR_ROJO "CANCELADA" COLOR_RESET); break;
	case ESTADO_AUSENTE: strcpy(estadoStr, COLOR_AMARILLO "AUSENTE (No asistio)" COLOR_RESET); break;
	default: strcpy(estadoStr, COLOR_AZUL "REALIZADA (Atendido)" COLOR_RESET); break;
	}
	printf("   Estado:   %s\n\n", estadoStr);
	
	if (cita.estado == ESTADO_REALIZADA) {
		printf(COLOR_CYAN "   [ SIGNOS VITALES / TRIAGE ]\n" COLOR_RESET);
		printf("   Peso:               %.2f kg\n", cita.peso);
		printf("   Frecuencia Card.:   %d lpm\n", cita.frec_cardiaca);
		printf("   Presion Arterial:   %s\n\n", cita.presion);
		
		printf(COLOR_CYAN "   [ SINTOMAS REPORTADOS ]\n" COLOR_RESET);
		printf("   %s\n\n", cita.sintomas);
		
		printf(COLOR_CYAN "   [ DIAGNOSTICO Y RECETA ]\n" COLOR_RESET);
		printf("   %s\n\n", cita.diagnostico);
	} else {
		printf(COLOR_AMARILLO "   [!] Esta cita aun no cuenta con historial clinico llenado por el medico.\n" COLOR_RESET);
		printf("       (Solo estara disponible despues de que ocurra la consulta)\n\n");
	}
	
	pausar();
}

// ==========================================
// 5. VISTAS Y LISTADOS (MÉDICOS Y PACIENTES)
// ==========================================

void verHistorialPaciente(const Cita citas[], int n_citas, const Paciente pacientes[], int n_pacientes, const Medico medicos[], int n_medicos, char* cedulaPaciente) {
	int idSeleccionado;
	do {
		limpiarPantalla();
		dibujarEncabezado("MIS CITAS E HISTORIAL MEDICO");
		printf("   Paciente C.I: " NEGRITA "%s" COLOR_RESET "\n", cedulaPaciente);
		
		printf(COLOR_CYAN "   ========================================================================\n" COLOR_RESET);
		printf(NEGRITA "   %-4s | %-11s | %-6s | %-18s | %-10s\n" COLOR_RESET, "ID", "FECHA", "HORA", "MEDICO", "ESTADO");
		printf(COLOR_CYAN "   ========================================================================\n" COLOR_RESET);
		
		int count = 0;
		char cedTmp[20]; strcpy(cedTmp, cedulaPaciente); limpiarString(cedTmp);
		
		for(int i = 0; i < n_citas; i++) {
			char citaCedTmp[20]; strcpy(citaCedTmp, citas[i].cedula_paciente); limpiarString(citaCedTmp);
			
			if (strcmp(citaCedTmp, cedTmp) == 0) {
				char estadoStr[15], color[10], nombreMed[20] = "Desconocido";
				
				char citaMedTmp[20]; strcpy(citaMedTmp, citas[i].codigo_medico); limpiarString(citaMedTmp);
				for(int m = 0; m < n_medicos; m++) {
					char medTmp2[20]; strcpy(medTmp2, medicos[m].codigo); limpiarString(medTmp2);
					if(strcmp(medTmp2, citaMedTmp) == 0) {
						strncpy(nombreMed, medicos[m].nombre, 17); nombreMed[17] = '\0'; break;
					}
				}
				
				switch(citas[i].estado) {
				case ESTADO_ACTIVA:    strcpy(estadoStr, "ACTIVA"); strcpy(color, COLOR_VERDE); break;
				case ESTADO_CANCELADA: strcpy(estadoStr, "CANCELADA"); strcpy(color, COLOR_ROJO); break;
				case ESTADO_AUSENTE:   strcpy(estadoStr, "FALTO"); strcpy(color, COLOR_AMARILLO); break;
				default:               strcpy(estadoStr, "REALIZADA"); strcpy(color, COLOR_AZUL); break;
				}
				
				printf("   %-4d | %-11s | %-6s | %-18s | %s%-10s" COLOR_RESET "\n", 
					   citas[i].id, citas[i].fecha, citas[i].hora, nombreMed, color, estadoStr);
				count++;
			}
		}
		
		if (count == 0) {
			printf(COLOR_AMARILLO "   (No tiene citas registradas en el sistema)\n" COLOR_RESET);
			printf(COLOR_CYAN "   ========================================================================\n" COLOR_RESET);
			pausar();
			break; 
		}
		printf(COLOR_CYAN "   ========================================================================\n" COLOR_RESET);
		
		idSeleccionado = leerEntero("\n   > Ingrese ID para leer FICHA COMPLETA (0 para regresar al Menu): ");
		if (idSeleccionado != 0) {
			int idx = -1;
			for (int i = 0; i < n_citas; i++) {
				char citaCedTmp[20]; strcpy(citaCedTmp, citas[i].cedula_paciente); limpiarString(citaCedTmp);
				if (citas[i].id == idSeleccionado && strcmp(citaCedTmp, cedTmp) == 0) {
					idx = i; break;
				}
			}
			if (idx != -1) {
				verDetalleFicha(citas[idx], pacientes, n_pacientes, medicos, n_medicos);
			} else {
				imprimirError("ID no valido o no le pertenece.");
				pausar();
			}
		}
	} while (idSeleccionado != 0);
}

void verCitasDeMedico(const Cita citas[], int n_citas, const Paciente pacientes[], int n_pacientes, const Medico medicos[], int n_medicos, char* codigoMedico) {
	int idSeleccionado;
	char medTmp[20]; strcpy(medTmp, codigoMedico); limpiarString(medTmp);
	
	do {
		limpiarPantalla();
		dibujarEncabezado("MI AGENDA E HISTORIAL DE PACIENTES");
		printf("   Medico: " NEGRITA "%s" COLOR_RESET "\n", codigoMedico);
		
		printf(COLOR_CYAN "   ========================================================================\n" COLOR_RESET);
		printf(NEGRITA "   %-4s | %-11s | %-6s | %-18s | %-10s\n" COLOR_RESET, "ID", "FECHA", "HORA", "PACIENTE", "ESTADO");
		printf(COLOR_CYAN "   ========================================================================\n" COLOR_RESET);
		
		int count = 0;
		for(int i=0; i<n_citas; i++) {
			char citaMedTmp[20]; strcpy(citaMedTmp, citas[i].codigo_medico); limpiarString(citaMedTmp);
			
			if (strcmp(citaMedTmp, medTmp) == 0) {
				char estadoStr[15], color[10], nombrePac[20] = "Desconocido";
				
				int idxP = buscarPacientePorCedula(pacientes, n_pacientes, citas[i].cedula_paciente);
				if(idxP != -1) { strncpy(nombrePac, pacientes[idxP].nombre, 17); nombrePac[17] = '\0'; }
				
				switch(citas[i].estado) {
				case ESTADO_ACTIVA:    strcpy(estadoStr, "ACTIVA"); strcpy(color, COLOR_VERDE); break;
				case ESTADO_CANCELADA: strcpy(estadoStr, "CANCEL"); strcpy(color, COLOR_ROJO); break;
				case ESTADO_AUSENTE:   strcpy(estadoStr, "AUSENTE"); strcpy(color, COLOR_AMARILLO); break;
				default:               strcpy(estadoStr, "REALIZADA"); strcpy(color, COLOR_AZUL); break;
				}
				
				printf("   %-4d | %-11s | %-6s | %-18s | %s%-10s" COLOR_RESET "\n", 
					   citas[i].id, citas[i].fecha, citas[i].hora, nombrePac, color, estadoStr);
				count++;
			}
		}
		
		if (count == 0) {
			printf(COLOR_AMARILLO "   (No tiene citas en el historial)\n" COLOR_RESET);
			printf(COLOR_CYAN "   ========================================================================\n" COLOR_RESET);
			pausar();
			break;
		}
		printf(COLOR_CYAN "   ========================================================================\n" COLOR_RESET);
		
		idSeleccionado = leerEntero("\n   > Ingrese ID para leer FICHA COMPLETA (0 para regresar al Menu): ");
		if (idSeleccionado != 0) {
			int idx = -1;
			for (int i = 0; i < n_citas; i++) {
				char citaMedTmp[20]; strcpy(citaMedTmp, citas[i].codigo_medico); limpiarString(citaMedTmp);
				if (citas[i].id == idSeleccionado && strcmp(citaMedTmp, medTmp) == 0) {
					idx = i; break;
				}
			}
			if (idx != -1) {
				verDetalleFicha(citas[idx], pacientes, n_pacientes, medicos, n_medicos);
			} else {
				imprimirError("ID no valido o no pertenece a su agenda.");
				pausar();
			}
		}
	} while (idSeleccionado != 0);
}

// ==========================================
// 6. OPERACIONES EXTRAS (REAGENDAR / CANCELAR)
// ==========================================

void reagendarCita(Cita citas[], int n_citas, const Medico medicos[], int n_medicos, int rol, char* usuarioActual) {
	dibujarEncabezado("REAGENDAR CITA");
	char userTmp[20]; strcpy(userTmp, usuarioActual); limpiarString(userTmp);
	
	if (rol == 3) { 
		printf("   Sus citas activas:\n");
		int count = 0;
		for(int i=0; i<n_citas; i++) {
			char citaCedTmp[20]; strcpy(citaCedTmp, citas[i].cedula_paciente); limpiarString(citaCedTmp);
			if(citas[i].estado == ESTADO_ACTIVA && strcmp(citaCedTmp, userTmp) == 0) {
				printf("   [ID: %d] %s %s - Dr. %s\n", citas[i].id, citas[i].fecha, citas[i].hora, citas[i].codigo_medico);
				count++;
			}
		}
		if(count == 0) { imprimirError("No tiene citas activas para reagendar."); pausar(); return; }
	} else {
		listarCitas(citas, n_citas); 
	}
	
	imprimirLinea(60);
	int id = leerEntero("   > Ingrese ID a modificar (0 salir): ");
	if (id == 0) return;
	
	int idxCita = -1;
	for (int i = 0; i < n_citas; i++) { if (citas[i].id == id) { idxCita = i; break; } }
	if (idxCita == -1) { imprimirError("No existe."); pausar(); return; }
	
	char citaCedTmp[20]; strcpy(citaCedTmp, citas[idxCita].cedula_paciente); limpiarString(citaCedTmp);
	if (rol == 3 && strcmp(citaCedTmp, userTmp) != 0) {
		imprimirError("Esta cita no le pertenece."); pausar(); return;
	}
	if (citas[idxCita].estado != ESTADO_ACTIVA) {
		imprimirError("Solo se pueden reagendar citas activas."); pausar(); return;
	}
	
	int idxMed = -1;
	char citaMedTmp[20]; strcpy(citaMedTmp, citas[idxCita].codigo_medico); limpiarString(citaMedTmp);
	for (int i = 0; i < n_medicos; i++) {
		char medTmp[20]; strcpy(medTmp, medicos[i].codigo); limpiarString(medTmp);
		if (strcmp(medTmp, citaMedTmp) == 0) { idxMed = i; break; }
	}
	if (idxMed == -1) { imprimirError("El medico no existe."); pausar(); return; }
	
	printf(COLOR_CYAN "\n   Actual: %s %s (Dr. %s)\n" COLOR_RESET, citas[idxCita].fecha, citas[idxCita].hora, medicos[idxMed].nombre);
	
	char nuevaFecha[11], nuevaHora[6];
	seleccionarFechaMenu(nuevaFecha, 14); 
	
	if(!seleccionarHoraMenu(citas, n_citas, citas[idxCita].codigo_medico, nuevaFecha, medicos[idxMed].horario_inicio, medicos[idxMed].horario_fin, nuevaHora)) return; 
	
	char conf;
	int opValida = 0;
	do {
		printf("   > ¿Confirmar reagendamiento? (s/n): ");
		fflush(stdin); scanf(" %c", &conf); while(getchar() != '\n'); 
		conf = tolower(conf);
		if (conf == 's' || conf == 'n') opValida = 1; else imprimirError("Ingrese 's' o 'n'.");
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

void cancelarCita(Cita citas[], int n_citas, int rol, char* usuarioActual) {
	dibujarEncabezado("CANCELAR CITA");
	char userTmp[20]; strcpy(userTmp, usuarioActual); limpiarString(userTmp);
	
	if (rol == 3) { 
		printf("   Sus citas activas:\n");
		int count = 0;
		for(int i=0; i<n_citas; i++) {
			char citaCedTmp[20]; strcpy(citaCedTmp, citas[i].cedula_paciente); limpiarString(citaCedTmp);
			if(citas[i].estado == ESTADO_ACTIVA && strcmp(citaCedTmp, userTmp) == 0) {
				printf("   [ID: %d] %s - %s (Dr. %s)\n", citas[i].id, citas[i].fecha, citas[i].hora, citas[i].codigo_medico);
				count++;
			}
		}
		if(count == 0) { printf("   No tiene citas activas para cancelar.\n"); pausar(); return; }
	} else {
		listarCitas(citas, n_citas); 
	}
	
	int id = leerEntero("   > Ingrese ID de cita a cancelar (0 salir): ");
	if (id == 0) return;
	
	int idx = -1;
	for (int i = 0; i < n_citas; i++) { if (citas[i].id == id) { idx = i; break; } }
	
	if (idx == -1) { imprimirError("Cita no encontrada."); pausar(); return; }
	
	char citaCedTmp[20]; strcpy(citaCedTmp, citas[idx].cedula_paciente); limpiarString(citaCedTmp);
	if (rol == 3 && strcmp(citaCedTmp, userTmp) != 0) { imprimirError("Esa cita no es suya."); pausar(); return; }
	
	if (citas[idx].estado != ESTADO_ACTIVA) { imprimirError("La cita ya no esta activa."); pausar(); return; }
	
	citas[idx].estado = ESTADO_CANCELADA;
	imprimirExito("Cita cancelada correctamente.");
	pausar();
}

void registrarAusencia(Cita citas[], int n_citas, int rol, char* usuarioActual) {
	dibujarEncabezado("REGISTRAR AUSENCIA");
	char userTmp[20]; strcpy(userTmp, usuarioActual); limpiarString(userTmp);
	
	for(int i=0; i<n_citas; i++) {
		char citaMedTmp[20]; strcpy(citaMedTmp, citas[i].codigo_medico); limpiarString(citaMedTmp);
		if(citas[i].estado == ESTADO_ACTIVA && strcmp(citaMedTmp, userTmp) == 0) {
			printf("   [ID: %d] %s %s - C.I Paciente: %s\n", citas[i].id, citas[i].fecha, citas[i].hora, citas[i].cedula_paciente);
		}
	}
	
	int id = leerEntero("\n   > ID de la cita donde paciente FALTO: ");
	
	int idx = -1;
	for (int i = 0; i < n_citas; i++) { if (citas[i].id == id) { idx = i; break; } }
	
	if (idx == -1) { imprimirError("No existe ese ID."); pausar(); return; }
	
	char citaMedTmp[20]; strcpy(citaMedTmp, citas[idx].codigo_medico); limpiarString(citaMedTmp);
	if (strcmp(citaMedTmp, userTmp) != 0 && rol != 1) { imprimirError("No es de su agenda."); pausar(); return; }
	
	if (citas[idx].estado != ESTADO_ACTIVA) { imprimirError("La cita no esta activa."); pausar(); return; }
	
	citas[idx].estado = ESTADO_AUSENTE;
	imprimirExito("Paciente marcado como AUSENTE.");
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
		case ESTADO_CANCELADA: strcpy(est, "CANCEL"); strcpy(col, COLOR_ROJO); break;
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

// ==========================================
// 7. PERSISTENCIA (GUARDADO Y LECTURA)
// ==========================================

void cargarCitas(Cita citas[], int *cantidad) {
	FILE *fp = fopen(ARCHIVO_CITAS, "r");
	*cantidad = 0;
	if (!fp) return;
	
	char linea[2500]; 
	while (fgets(linea, sizeof(linea), fp)) {
		if (*cantidad >= MAX_CITAS) break;
		Cita c;
		
		int leidos = sscanf(linea, "%d;%[^;];%[^;];%[^;];%[^;];%d;%f;%d;%[^;];%[^;];%[^\n]",
							&c.id, c.cedula_paciente, c.codigo_medico, 
							c.fecha, c.hora, &c.estado,
							&c.peso, &c.frec_cardiaca, c.presion, c.sintomas, c.diagnostico);
		
		if (leidos == 6) { 
			c.peso = 0.0; c.frec_cardiaca = 0;
			strcpy(c.presion, "N/A"); strcpy(c.sintomas, "N/A"); strcpy(c.diagnostico, "Pendiente");
		}
		
		if (leidos >= 6) { 
			// ?? EL TRUCO MÁGICO: Forzamos a que el ID sea perfecto y ordenado (1, 2, 3...)
			// Esto ignora el número gigante generado por la IA
			c.id = (*cantidad) + 1; 
			
			citas[*cantidad] = c; 
			(*cantidad)++; 
		}
	}
	fclose(fp);
}

void guardarCitas(const Cita citas[], int cantidad) {
	FILE *fp = fopen(ARCHIVO_CITAS, "w");
	if (!fp) {
		printf("\n");
		printf(COLOR_ROJO "========================================================\n" COLOR_RESET);
		printf(COLOR_ROJO " [ALERTA CRITICA] NO SE PUDO GUARDAR EN LA BASE DE DATOS\n" COLOR_RESET);
		printf("========================================================\n");
		printf(" Motivo: El sistema no encuentra la carpeta '/data/'.\n");
		printf(" Solucion: Vaya a la carpeta donde esta su programa y \n");
		printf(" cree una nueva carpeta llamada exactamente: data\n");
		printf(COLOR_ROJO "========================================================\n" COLOR_RESET);
		pausar();
		return;
	}
	for (int i = 0; i < cantidad; i++) {
		fprintf(fp, "%d;%s;%s;%s;%s;%d;%.2f;%d;%s;%s;%s\n",
				citas[i].id, citas[i].cedula_paciente, citas[i].codigo_medico,
				citas[i].fecha, citas[i].hora, citas[i].estado,
				citas[i].peso, citas[i].frec_cardiaca, citas[i].presion, 
				citas[i].sintomas, citas[i].diagnostico);
	}
	fclose(fp);
}

void inicializarCitas(Cita citas[], int *cantidad) { *cantidad = 0; }
