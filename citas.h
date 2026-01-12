#ifndef CITAS_H
#define CITAS_H

#include "config.h"
#include "medicos.h"
#include "pacientes.h"

#define ESTADO_CANCELADA 0
#define ESTADO_ACTIVA    1
#define ESTADO_REALIZADA 2
#define ESTADO_AUSENTE   3

typedef struct {
	int id;
	char cedula_paciente[11];
	char codigo_medico[10];
	char fecha[11];
	char hora[6];
	int estado; 
} Cita;

void cargarCitas(Cita citas[], int *cantidad);
void guardarCitas(const Cita citas[], int cantidad);

void inicializarCitas(Cita citas[], int *cantidad);
void agendarCita(Cita citas[], int *n_citas, 
				 const Paciente pacientes[], int n_pacientes,
				 const Medico medicos[], int n_medicos,
				 int rol, char* usuarioActual);

void cancelarCita(Cita citas[], int n_citas, int rol, char* usuarioActual);
void registrarAusencia(Cita citas[], int n_citas, int rol, char* usuarioActual);

void verCitasDeMedico(const Cita citas[], int n_citas, char* codigoMedico);
void listarCitas(const Cita citas[], int n_citas);

#endif
