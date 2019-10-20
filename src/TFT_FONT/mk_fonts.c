/*
 * mk_fonts.c
 *
 *  Created on: 2011-12-22
 *      Author: Miros³aw Kardaœ
 */


#include "mk_fonts.h"
//#include "../MK_SSD1963.h"



#if USE_PETIT_FAT == 1
#include "../../PetitFS/diskio.h"
#include "../../PetitFS/pff.h"

/* array of short file names for Petit Fat */
char FontNames[][13]  = {
    {"Calib18.bin"},
    {"Tahom48.bin"},

};

#endif
