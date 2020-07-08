/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/***************************************************************************
 *
 *   Enrico Fucile
 *   Modified for Performance Study by: CS GMBH
 *
 ***************************************************************************/

#include "grib_api_internal.h"

extern grib_vsarrayPOOL vsarrayPOOL;

int grib_vsarray_init(grib_vsarray* source)
{

	if (!source)
		return 0;

	memset(source->stA,0,sizeof(grib_sarray*)*DYN_DEFAULT_VSARRAY_SIZE_INIT);
	source->size 				= DYN_DEFAULT_VSARRAY_SIZE_INIT;
	source->incsize             = DYN_DEFAULT_VSARRAY_SIZE_INCR;
	source->n					= 0;
	source->dynA				= 0;

	return 1;
}

void grib_vsarray_free_dynamic (grib_context* c, grib_vsarray* source)
{
	if (!source)
		return;

	if (source->dynA)
		grib_context_free(c, source->dynA);

}

grib_vsarray* grib_vsarray_new(grib_context* c, size_t size, size_t incsize)
{
	grib_vsarray* result = NULL;

	if (!c)
		c = grib_context_get_default();

	result = (grib_vsarray*)grib_context_malloc_clear(c, sizeof(grib_vsarray));

	if (!result) {
		grib_context_log(c, GRIB_LOG_ERROR,
				"grib_vsarray_new unable to allocate %d bytes\n", sizeof(grib_vsarray));
		return NULL;
	}

	if(size > DYN_DEFAULT_VSARRAY_SIZE_INIT) {

		result->dynA       = (grib_sarray**)grib_context_malloc_clear(c, sizeof(grib_sarray*) *  (size-DYN_DEFAULT_VSARRAY_SIZE_INIT)  );

		if (!result->dynA) {
			grib_context_log(c, GRIB_LOG_ERROR,
					"grib_vsarray_new unable to allocate %d bytes\n", sizeof(grib_sarray*) * (size-DYN_DEFAULT_VSARRAY_SIZE_INIT)  );
			return NULL;
		}
		result->size    = size;

		if (incsize > 0)
			result->incsize             = incsize;
		else
			result->incsize             = DYN_DEFAULT_VSARRAY_SIZE_INCR;

	} else {

		result->dynA					= NULL;
		result->size                = DYN_DEFAULT_VSARRAY_SIZE_INIT;
		result->incsize             = DYN_DEFAULT_VSARRAY_SIZE_INCR;

	}

	result->n       = 0;

	return result;
}

grib_vsarray* grib_vsarray_resize(grib_context* c, grib_vsarray* origin)
{
	int newsize;

	if (!origin)
	{
		return origin;
	}

	newsize = origin->incsize + origin->size;

	if (!c)
		c = grib_context_get_default();

	if (origin->dynA){

		origin->dynA    = (grib_sarray**)grib_context_realloc(c, origin->dynA, (newsize-DYN_DEFAULT_VSARRAY_SIZE_INIT) * sizeof(grib_sarray*));
		if (!origin->dynA) {
			grib_context_log(c, GRIB_LOG_ERROR,
					"grib_vsarray_resize unable to allocate %d bytes\n", sizeof(grib_sarray*) * (newsize-DYN_DEFAULT_VSARRAY_SIZE_INIT) );
			return NULL;
		}

	}
	else {

		origin->dynA = (grib_sarray**)grib_context_malloc(c, (newsize-DYN_DEFAULT_VSARRAY_SIZE_INIT) * sizeof(grib_sarray*));
		if (!origin->dynA) {
			grib_context_log(c, GRIB_LOG_ERROR,
					"grib_sarray_resize unable to allocate %d bytes\n", sizeof(grib_sarray*) * (newsize-DYN_DEFAULT_SARRAY_SIZE_INIT) );
			return NULL;
		}

	}

	origin->size = newsize;

	return origin;
}

void vsarrayPoolInit()
{
	int i;

	for (i=0; i<DYN_DEFAULT_VSARRAY_POOL_SIZE; i++) {
		vsarrayPOOL.arrayPOOL[i].size=DYN_DEFAULT_VSARRAY_SIZE_INIT;
		vsarrayPOOL.arrayPOOL[i].n=0;
		vsarrayPOOL.arrayPOOL[i].incsize=DYN_DEFAULT_VSARRAY_SIZE_INCR;
		vsarrayPOOL.arrayPOOL[i].dynA=0;
	}
}

grib_vsarray* grib_vsarray_push(grib_context* c, grib_vsarray* source, grib_sarray* val)
{
	size_t start_size    = DYN_DEFAULT_VSARRAY_SIZE_INIT;
	size_t start_incsize = DYN_DEFAULT_VSARRAY_SIZE_INCR;

	/*If the target is empty, initialize it*/
	if (!source)
		source = grib_vsarray_new(c, start_size, start_incsize);

	/*if (!source) {
		if(vsarrayPOOL.poolCounter != (DYN_DEFAULT_VSARRAY_POOL_SIZE - 1) ) {
			source = &(vsarrayPOOL.arrayPOOL[vsarrayPOOL.poolCounter]);
			vsarrayPOOL.poolCounter++;
		} else {
			source = grib_vsarray_new(0, start_size, start_incsize);
		}
	}*/

	/*If the actual used size of the target is equal to the allowed size, resize the array*/
	if (source->n == source->size) {

		source = grib_vsarray_resize(c, source);

		/*Check if REALLOC WAS POSSIBLE, otherwise it is not possible to insert a new value!*/
		if (!source)
		{
			return source;
		}

		/*insertion in new allocated dynamic array*/
		source->dynA[ (source->n-(DYN_DEFAULT_VSARRAY_SIZE_INIT)) ] = val;
		source->n++;
		return source;

	}

	/*insertion in static array*/
	if (source->n <= (DYN_DEFAULT_VSARRAY_SIZE_INIT - 1) )
	{
		source->stA[source->n] = val;
	}
	else {
		/*insertion in dynamic array*/
		source->dynA[ (source->n-(DYN_DEFAULT_VSARRAY_SIZE_INIT)) ] = val;
	}

	source->n++;

	return source;
}

/**
 * Return the grib_sarray* value at index 'index' for grib_vsarray 'source'
 */
grib_sarray* grib_vsarray_get(const grib_vsarray* source, size_t index)
{

	if (!source)
		return NULL;
	if (index < 0)
		return NULL;
	if (index < DYN_DEFAULT_VSARRAY_SIZE_INIT) {
		return source->stA[index];
	} else {
		if (source->dynA) {
			return source->dynA[(index-DYN_DEFAULT_VSARRAY_SIZE_INIT)];
		}
	}

	return NULL;
}

/**
 * Sets the grib_sarray* value 'val' at index 'index' for grib_vsarray 'source'.
 * The method overwrites the existing value, if any.
 * The method returns 1 if the writing operation occurred, otherwise 0.
 */
int grib_vsarray_put (grib_vsarray* source, size_t index, grib_sarray* val)
{
	if (!source)
		return 0;
	if (index < 0)
		return 0;
	if (index < DYN_DEFAULT_VSARRAY_SIZE_INIT) {
		source->stA[index] = val;

	} else {
		if (source->dynA) {
			source->dynA[(index-DYN_DEFAULT_VSARRAY_SIZE_INIT)] = val;
		}
	}

	return 1;
}

void grib_vsarray_delete(grib_context* c, grib_vsarray* source)
{
	if (!source)
		return;
	if (!c)
		grib_context_get_default();

	if (source->dynA)
		grib_context_free(c, source->dynA);

	grib_context_free(c, source);
}

void grib_vsarray_delete_content(grib_context* c, grib_vsarray* source)
{
	int i, statSize;

	if (!source)
		return;

	if(source->n){
		/*If the actual used size of the target is greater than the allowed size, it is a resized array*/
		if (source->n > DYN_DEFAULT_VSARRAY_SIZE_INIT && ( source->dynA ) ) {

			for (i =0; i<source->n - DYN_DEFAULT_VSARRAY_SIZE_INIT;i++){
				grib_sarray_delete(c, source->dynA[i]);
			}
			statSize = DYN_DEFAULT_VSARRAY_SIZE_INIT;
		} else {

			if (source->n == DYN_DEFAULT_VSARRAY_SIZE_INIT){
				statSize = DYN_DEFAULT_VSARRAY_SIZE_INIT;
			}else {
				statSize = source->n;
			}

		}

		for (i =0; i<statSize; i++){
			grib_sarray_delete(c, source->stA[i]);
		}

		source->n = 0;
	}
}

size_t grib_vsarray_used_size(grib_vsarray* source)
{
	return (!source) ? 0 : source->n;
}
