/*
 * controldata_utils.h
 *		Common code for mdb_controldata output
 *
 *	Portions Copyright (c) 1996-2016, MollyDB Global Development Group
 *	Portions Copyright (c) 1994, Regents of the University of California
 *
 *	src/include/common/controldata_utils.h
 */
#ifndef COMMON_CONTROLDATA_UTILS_H
#define COMMON_CONTROLDATA_UTILS_H

#include "catalog/mdb_control.h"

extern ControlFileData *get_controlfile(char *DataDir, const char *progname);

#endif   /* COMMON_CONTROLDATA_UTILS_H */
