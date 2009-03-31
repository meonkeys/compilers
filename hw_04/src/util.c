
#include "util.h"
#include <stdlib.h>

void free_const(Const_type* ct){
	if(ct->const_type == SC){
		free(ct->const_u.sc);
	}
	free(ct);
}
