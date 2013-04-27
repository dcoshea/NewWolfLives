/*************************************************************************
** NewWolf
** Copyright (C) 1999-2002 by DarkOne
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**************************************************************************
** wolf data files I/O
*************************************************************************/
#include "common.h"

typedef	struct chunk_s
{
	int offset;
	int size;
}	chunk_t;

typedef struct pagefile_s
{
	word totalChunks;
	word startSprites;
	word startSounds;
	char filename[MAX_OSPATH];
	chunk_t *chunks;
	FILE *handle;
}	pagefile_t;

typedef struct wolf_fs_s
{
	pagefile_t *swap;
} wolf_fs_t;

wolf_fs_t wolf_fs;

// ------------------------- * VSWAP (PageFile) * -------------------------

/*
** FS_PageFileLink
*/
pagefile_t *FS_PageFileLink(char *name)
{
	FILE *swaphandle;
	pagefile_t *pagefile;
	int n;

	swaphandle=fopen(name, "rb");
	if(!swaphandle) return NULL;

	pagefile=malloc(sizeof(pagefile_t));
	if(!pagefile)
	{
		fclose(swaphandle);
		return NULL;
	}

// Read in header variables
	fread(&pagefile->totalChunks, sizeof(word), 1, swaphandle);
	fread(&pagefile->startSprites, sizeof(word), 1, swaphandle);
	fread(&pagefile->startSounds, sizeof(word), 1, swaphandle);

	if(pagefile->startSprites>pagefile->startSounds || pagefile->startSounds>pagefile->totalChunks)
	{
		free(pagefile);
		fclose(swaphandle);
		return NULL;
	}

	pagefile->chunks=calloc(pagefile->totalChunks, sizeof(chunk_t));
	for(n=0; n<pagefile->totalChunks; n++)
		fread(&pagefile->chunks[n].offset, sizeof(long), 1, swaphandle);
	for(n=0; n<pagefile->totalChunks; n++)
		fread(&pagefile->chunks[n].size, sizeof(word), 1, swaphandle);

	strcpy(pagefile->filename, name);
	pagefile->handle=swaphandle;

	return pagefile;
}

/*
** FS_PageFileUnLink
*/
void FS_PageFileUnLink(pagefile_t *pagefile)
{
	if(pagefile->chunks) free pagefile->chunks;
	if(pagefile->handle) fclose(pagefile->handle);
	free(pagefile);
}

/*
** FS_PageFileRead
*/
int FS_PageFileRead(void **data, int page, pagefile_t *pagefile)
{
	int size;

	if(!pagefile || !pagefile->handle || page>=pagefile->chunks)
		return -1;

	size=pagefile->chunks[page].size;
	if(!size) return -1;

	if(data)
	{
		*data=malloc(size);
		fseek(pagefile->handle, pagefile->chunks[page].offset, SEEK_SET);
		fread(*data, 1, size, pagefile->handle);
	}
	return size;
}

/*
** FS_PageFileReadWall
*/
int FS_PageFileReadWall(int page, pagefile_t *pagefile)
