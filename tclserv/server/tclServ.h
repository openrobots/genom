/*
 * Copyright (C) 1999 LAAS/CNRS 
 *
 * Anthony Mallet - Fri Mar 26 1999
 *
 * $Source$
 * $Revision$
 * $Date$
 * 
 */
#ifndef _TCLSERV_H
#define _TCLSERV_H

#include <stdarg.h>
#include <tcl.h>

#define TCLSERV_CMDPORT		9472
#define TCLSERV_PRIORITY	110

STATUS		tclServ(char *);

int		Tcl_AppInit(Tcl_Interp *interp);
int		TclServExit(ClientData, Tcl_Interp *, int,
						Tcl_Obj *const []);

int		Tcl_GetEnumFromObj(Tcl_Interp *, Tcl_Obj *, int *, ...);
Tcl_Obj *	Tcl_NewEnumObj(Tcl_Interp *interp, int value, ...);

int		tclServInstallAsync(Tcl_Interp *interp);

#endif /* _TCLSERV_H */
