/*
 * Dynamic loading support for Darwin
 *
 * If dlopen() is available (Darwin 10.3 and later), we just use it.
 * Otherwise we emulate it with the older, now deprecated, NSLinkModule API.
 *
 * src/backend/port/dynloader/darwin.c
 */
#include "mollydb.h"

#ifdef HAVE_DLOPEN
#include <dlfcn.h>
#else
#include <mach-o/dyld.h>
#endif

#include "dynloader.h"


#ifdef HAVE_DLOPEN

void *
mdb_dlopen(char *filename)
{
	return dlopen(filename, RTLD_NOW | RTLD_GLOBAL);
}

void
mdb_dlclose(void *handle)
{
	dlclose(handle);
}

PGFunction
mdb_dlsym(void *handle, char *funcname)
{
	/* Do not prepend an underscore: see dlopen(3) */
	return dlsym(handle, funcname);
}

char *
mdb_dlerror(void)
{
	return dlerror();
}
#else							/* !HAVE_DLOPEN */

/*
 * These routines were taken from the Apache source, but were made
 * available with a MollyDB-compatible license.  Kudos Wilfredo
 * Sánchez <wsanchez@apple.com>.
 */

static NSObjectFileImageReturnCode cofiff_result = NSObjectFileImageFailure;

void *
mdb_dlopen(char *filename)
{
	NSObjectFileImage image;

	cofiff_result = NSCreateObjectFileImageFromFile(filename, &image);
	if (cofiff_result != NSObjectFileImageSuccess)
		return NULL;
	return NSLinkModule(image, filename,
						NSLINKMODULE_OPTION_BINDNOW |
						NSLINKMODULE_OPTION_RETURN_ON_ERROR);
}

void
mdb_dlclose(void *handle)
{
	NSUnLinkModule(handle, FALSE);
}

PGFunction
mdb_dlsym(void *handle, char *funcname)
{
	NSSymbol symbol;
	char	   *symname = (char *) malloc(strlen(funcname) + 2);

	sprintf(symname, "_%s", funcname);
	if (NSIsSymbolNameDefined(symname))
	{
		symbol = NSLookupAndBindSymbol(symname);

		free(symname);
		return (PGFunction) NSAddressOfSymbol(symbol);
	}
	else
	{
		free(symname);
		return NULL;
	}
}

char *
mdb_dlerror(void)
{
	NSLinkEditErrors c;
	int			errorNumber;
	const char *fileName;
	const char *errorString = NULL;

	switch (cofiff_result)
	{
		case NSObjectFileImageSuccess:
			/* must have failed in NSLinkModule */
			NSLinkEditError(&c, &errorNumber, &fileName, &errorString);
			if (errorString == NULL || *errorString == '\0')
				errorString = "unknown link-edit failure";
			break;
		case NSObjectFileImageFailure:
			errorString = "failed to open object file";
			break;
		case NSObjectFileImageInappropriateFile:
			errorString = "inappropriate object file";
			break;
		case NSObjectFileImageArch:
			errorString = "object file is for wrong architecture";
			break;
		case NSObjectFileImageFormat:
			errorString = "object file has wrong format";
			break;
		case NSObjectFileImageAccess:
			errorString = "insufficient permissions for object file";
			break;
		default:
			errorString = "unknown failure to open object file";
			break;
	}

	return (char *) errorString;
}

#endif   /* HAVE_DLOPEN */
