/*	$LAAS$ */

/* 
 * Copyright (c) 1999-2003 LAAS/CNRS                    - Fri Mar 26 1999
 * All rights reserved.
 *
 * Redistribution and use  in source  and binary  forms,  with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *   1. Redistributions of  source  code must retain the  above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice,  this list of  conditions and the following disclaimer in
 *      the  documentation  and/or  other   materials provided  with  the
 *      distribution.
 *
 * THIS  SOFTWARE IS PROVIDED BY  THE  COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND  ANY  EXPRESS OR IMPLIED  WARRANTIES,  INCLUDING,  BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES  OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR  PURPOSE ARE DISCLAIMED. IN  NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR      CONTRIBUTORS  BE LIABLE FOR   ANY    DIRECT, INDIRECT,
 * INCIDENTAL,  SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE   OF THIS SOFTWARE, EVEN   IF ADVISED OF   THE POSSIBILITY OF SUCH
 * DAMAGE.
 */
#include "genom-config.h"
__RCSID("$LAAS$");

#ifdef VXWORKS
# include <vxWorks.h>
#else
# include <portLib.h>
#endif

#include <string.h>

#include "tclServ.h"

/*
 * Convert an enum string rep. into an int ------------------------------
 */

/* the va_list is a list of 'string,value' pairs corresponding to the enum
 * 'symbolic,numeric' values */

int
Tcl_GetEnumFromObj(Tcl_Interp *interp, Tcl_Obj *obj, int *intPtr, ...)
{
   int enumValue, enumValueOk = TCL_ERROR;
   char *enumText, *str;
   va_list args;

   enumText = Tcl_GetStringFromObj(obj, NULL);

   /* match strings */
   va_start(args, intPtr);
   for(str=va_arg(args, char *); str!=NULL; str=va_arg(args, char *)) {
      enumValue = va_arg(args, int);

      if (strcmp(enumText, str) == 0) {
	 *intPtr = enumValue;
	 enumValueOk = TCL_OK;
	 break;
      }
   }
   va_end(args);

   if (enumValueOk == TCL_OK) return TCL_OK;
   if (Tcl_GetIntFromObj(NULL, obj, &enumValue) != TCL_OK)
      return TCL_ERROR;

   /* match integer values */
   va_start(args, intPtr);
   for(str=va_arg(args, char *); str!=NULL; str=va_arg(args, char *)) {

      if (enumValue == va_arg(args, int)) {
	 *intPtr = enumValue;
	 enumValueOk = TCL_OK;
	 break;
      }
   }
   va_end(args);
   
   if (enumValueOk == TCL_OK) return TCL_OK;

   if (interp != NULL) {
      Tcl_SetResult(interp, "Cannot convert ", TCL_STATIC);
      Tcl_AppendResult(interp, enumText, " into enum {\n", NULL); 

      va_start(args, intPtr);
      for(str=va_arg(args, char *); str!=NULL; str=va_arg(args, char *)) {
	 enumValue = va_arg(args, int);

	 Tcl_AppendResult(interp, "\t", str, "\n", NULL); 
      }
      va_end(args);

      Tcl_AppendResult(interp, "}", NULL); 
   }
   return TCL_ERROR;
}


/*
 * Convert an integer into an enum sting rep. ---------------------------
 */

/* the va_list is a list of 'string,value' pairs corresponding to the enum
 * 'symbolic,numeric' values */

Tcl_Obj *
Tcl_NewEnumObj(Tcl_Interp *interp, int value, ...)
{
   int enumValue;
   char *str;
   va_list args;

   /* match strings */
   va_start(args, value);
   for(str=va_arg(args, char *); str!=NULL; str=va_arg(args, char *)) {
      enumValue = va_arg(args, int);

      if (value == enumValue) {
	 return Tcl_NewStringObj(str, -1);
      }
   }
   va_end(args);

   if (interp != NULL) {
      char buf[20];
      
      sprintf(buf, "%d", value);
      Tcl_SetResult(interp, "Cannot convert \"", TCL_STATIC);
      Tcl_AppendResult(interp, buf, "\" into enum {\n", NULL); 

      va_start(args, value);
      for(str=va_arg(args, char *); str!=NULL; str=va_arg(args, char *)) {
	 enumValue = va_arg(args, int);

	 Tcl_AppendResult(interp, "\t", str, "\n", NULL); 
      }
      va_end(args);

      Tcl_AppendResult(interp, "}", NULL); 
   }
   return Tcl_NewIntObj(value);
}
