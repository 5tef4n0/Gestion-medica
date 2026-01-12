#ifndef PACIENTES_H
#define PACIENTES_H

#include "config.h"

typedef struct {
	char nombre[MAX_STR];
	char cedula[11];
	int edad;
	char telefono[20];
	char correo[MAX_STR];
	int activo;
} Paciente;

void inicializarPacientes(Paciente pacientes[], int *cantidad);
void registrarPaciente(Paciente pacientes[], int *cantidad); 
void listarPacientes(const Paciente pacientes[], int cantidad);
int buscarPacientePorCedula(const Paciente pacientes[], int cantidad, const char* cedula);

#endif
