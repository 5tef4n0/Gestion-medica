#ifndef AUTH_H
#define AUTH_H

#include "config.h"

int sistemaAutenticacion(char* usuarioActual);

void registrarUsuario();
int iniciarSesion(char* usuarioRetorno);

#endif
