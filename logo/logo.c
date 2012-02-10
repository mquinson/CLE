/* logo.c: main of the Logo lesson */

#include "logo/logo.h"
#include "logo/exercise_header.h"



lesson_t lesson_main() {
	return lesson_new("Logo",2,
			"Three squares", logo_threesquare_create,
			 "Forward", logo_forward_create);
}
