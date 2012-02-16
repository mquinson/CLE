#include "logo/logo.h"
#include "logo/exercise_header.h"

lesson_t lesson_main() {
	return lesson_new("Logo",5,
			"Threesquares", logo_threesquare_create,
			"Forward", logo_forward_create,
			"Square", logo_square_create,
			"One", logo_one_create,
			"Forward_Back", logo_forward_back_create);
}