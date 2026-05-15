#ifndef CITAS_H
#define CITAS_H

#include "config.h"
#include "medicos.h"
#include "pacientes.h"

// Estados
#define ESTADO_CANCELADA 0
#define ESTADO_ACTIVA    1
#define ESTADO_REALIZADA 2
#define ESTADO_AUSENTE   3

typedef struct {
	int id;
	char cedula_paciente[15];
	char codigo_medico[10];
	char fecha[11];
	char hora[6];
	int estado; 
	
	// --- HISTORIA CLINICA AMPLIADA ---
	float peso;              
	int frec_cardiaca;       
	char presion[15];        
	
	// Aumentados a 1000 caracteres para permitir textos muy largos
	char sintomas[1000];      
	char diagnostico[1000];   
} Cita;

// Persistencia
void cargarCitas(Cita citas[], int *cantidad);
void guardarCitas(const Cita citas[], int cantidad);
void inicializarCitas(Cita citas[], int *cantidad);

// Lógica Principal
void agendarCita(Cita citas[], int *n_citas, 
				 const Paciente pacientes[], int n_pacientes,
				 const Medico medicos[], int n_medicos,
				 int rol, char* usuarioActual);

// Operaciones
void cancelarCita(Cita citas[], int n_citas, int rol, char* usuarioActual);
void registrarAusencia(Cita citas[], int n_citas, int rol, char* usuarioActual);
void reagendarCita(Cita citas[], int n_citas, const Medico medicos[], int n_medicos, int rol, char* usuarioActual);

// Zona de atención
void atenderPaciente(Cita citas[], int n_citas, const Paciente pacientes[], int n_pacientes, char* codigoMedico);

// Vistas Mejoradas
void verCitasDeMedico(const Cita citas[], int n_citas, const Paciente pacientes[], int n_pacientes, const Medico medicos[], int n_medicos, char* codigoMedico);
void verHistorialPaciente(const Cita citas[], int n_citas, const Paciente pacientes[], int n_pacientes, const Medico medicos[], int n_medicos, char* cedulaPaciente);
void listarCitas(const Cita citas[], int n_citas);

#endif
