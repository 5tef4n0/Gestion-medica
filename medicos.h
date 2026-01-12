#ifndef MEDICOS_H
#define MEDICOS_H

#include "config.h"

typedef struct {
	char nombre[MAX_STR];
	char cedula[11];
	char codigo[10];     
	int edad;
	char especialidad[MAX_STR];
	char horario_inicio[6]; 
	char horario_fin[6];    
	int activo;
} Medico;

void inicializarMedicos(Medico medicos[], int *cantidad);
void registrarMedico(Medico medicos[], int *cantidad);
void listarMedicos(const Medico medicos[], int cantidad);
void modificarHorarioMedico(Medico medicos[], int cantidad); 

#endif
