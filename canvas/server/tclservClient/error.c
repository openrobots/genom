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

int $module$_decode_error(const char* str, $module$_error_t *err)
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
