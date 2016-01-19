#include "stub/misc.h"


FuncThunk<const char * (*)(const char *, int)> ft_TranslateWeaponEntForClass("TranslateWeaponEntForClass");
const char *TranslateWeaponEntForClass(const char *name, int classnum)
{
	return (*ft_TranslateWeaponEntForClass)(name, classnum);
}
