#include "logo/logo.h"
#include "logo/exercise_header.h"

lesson_t lesson_main() {
	return lesson_new("Logo",5
,			"Three squares", logo_threesquare_create
,			"Forward", logo_forward_create
,			"Square", logo_square_create
,			"One", logo_One_create
,			"Forward_Back", logo_Forward_Back_create);
}