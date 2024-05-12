#pragma once
# include "globals.h";


QString prune_version = "3.0";

// inDev = true (development) false (production)
bool inDev = false;

#include "functions/functions.h"
#include "NBTParser/parse_nbt.h"
#include "FXNTNBT/fxnt_nbt_types.h"
#include "FXNTNBT/fxnt_get_nbt.h"
#include "functions/world_init.h"
//#include "functions/world_functions.h"