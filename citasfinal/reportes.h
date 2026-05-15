#ifndef REPORTES_H
#define REPORTES_H

#include "pacientes.h"
#include "medicos.h"
#include "citas.h"

// Menú principal de reportes que llama a los demás
void menuReportes(const Cita citas[], int n_citas, 
				  const Paciente pacientes[], int n_pacientes, 
				  const Medico medicos[], int n_medicos);

// Reportes específicos
void reportePacientesRecurrentes(const Cita citas[], int n_citas, const Paciente pacientes[], int n_pacientes);
void reporteMedicosMasDemandados(const Cita citas[], int n_citas, const Medico medicos[], int n_medicos);
void reporteResumenCitasPorMedico(const Cita citas[], int n_citas, const Medico medicos[], int n_medicos);

#endif
