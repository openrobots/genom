#include <stdlib.h>

#include "$module$Error.h"

struct $module$_msg_error 
{
	const char* msg;
	$module$_error_t err;
};

static const struct $module$_msg_error const $module$TclservClientErrMsgs[] = {
	{ "OK", E_$module$_OK },
$listCntrlFailures$$listExecFailures$
$listStdFailures$
};

int $module$_decode_error(const char* str, int *err)
{
  int i;

  for (i = 0; i < sizeof($module$TclservClientErrMsgs) / sizeof($module$TclservClientErrMsgs[0]);
	   i++)
  {
	  if (strcmp($module$TclservClientErrMsgs[i].msg, str) == 0) {
		  *err = $module$TclservClientErrMsgs[i].err;
		  return 0;
	  }
  }

  return -1;
}

const char* $module$_error_str(int err)
{
  int i;

  if (err == E_$module$_OK) 
	  return "OK";

  for (i = 0; i < sizeof($module$TclservClientErrMsgs) / sizeof($module$TclservClientErrMsgs[0]);
	   i++)
  {
	  if (err == $module$TclservClientErrMsgs[i].err)  
		  return $module$TclservClientErrMsgs[i].msg;
  }

  return NULL;
}
