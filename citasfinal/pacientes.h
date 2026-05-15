#ifndef PACIENTES_H
#define PACIENTES_H

#include "config.h"

typedef struct {
	char cedula[15];
	char nombre[50];
	int edad;
	char telefono[15];
	char email[50]; // <-- NUEVO: Soporte para tu formato con correo
	int activo;
} Paciente;

// Funciones CRUD
void registrarPaciente(Paciente pacientes[], int *cantidad);
void listarPacientes(const Paciente pacientes[], int cantidad);
int buscarPacientePorCedula(const Paciente pacientes[], int cantidad, const char *cedula);

// Funciones de persistencia (Archivos)
void cargarPacientes(Paciente pacientes[], int *cantidad);
void guardarPacientes(const Paciente pacientes[], int cantidad);
void inicializarPacientes(Paciente pacientes[], int *cantidad);

#endif
