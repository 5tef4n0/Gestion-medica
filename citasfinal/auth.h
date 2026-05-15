#ifndef AUTH_H
#define AUTH_H

#include "pacientes.h"
#include "medicos.h"

#define ROL_ADMIN 1
#define ROL_MEDICO 2
#define ROL_PACIENTE 3

// Firma del sistema principal
int sistemaAutenticacion(char* usuarioActual, Paciente pacientes[], int *n_pacientes, Medico medicos[], int *n_medicos);

// NUEVA FIRMA: El sincronizador automatico
void sincronizarUsuariosBaseDeDatos(const Paciente pacientes[], int n_pacientes, const Medico medicos[], int n_medicos);

#endif
