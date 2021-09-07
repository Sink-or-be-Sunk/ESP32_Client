#pragma once

#include "cJSON.h"
#include <string.h>
#include <stdio.h>

//create a monitor with a list of supported resolutions
//NOTE: Returns a heap allocated string, you are required to free it after use.
char *create_new_game_req(void);