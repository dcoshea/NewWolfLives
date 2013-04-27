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
** file system
*************************************************************************/
#include "common.h"

// ------------------------- * types * -------------------------
typedef struct packfile_s
{
	char name[MAX_WLPATH];
	int filepos, filelen;
} packfile_t;

typedef struct pack_s
{
	char filename[MAX_OSPATH];
	FILE *handle;
	int numfiles;
	packfile_t *files;
} pack_t;

char fs_gamedir[MAX_OSPATH];
cvar_t *fs_basedir;
cvar_t *fs_gamedirvar;

typedef struct searchpath_s
{
	char filename[MAX_OSPATH];
	pack_t *pack; // only one of filename / pack will be used
	struct searchpath_s *next;
} searchpath_t;

searchpath_t *fs_searchpaths;
searchpath_t *fs_base_searchpaths; // without gamedirs

// ------------------------- * PAK files * -------------------------
#define IDPAKHEADER ('P'+('A'<<8)+('C'<<16)+('K'<<24))

typedef struct dpackfile_s
{
	char name[56];
	int filepos, filelen;
} dpackfile_t;

typedef struct dpackheader_s
{
	int ident;		//==IDPAKHEADER
	int dirofs;
	int dirlen;
} dpackheader_t;

#define	MAX_FILES_IN_PACK	4096

// ------------------------- * Devider * -------------------------

/*
** FS_filelength
*/
int FS_filelength(FILE *f)
{
	int pos, end;

	pos=ftell(f);
	fseek(f, 0, SEEK_END);
	end=ftell(f);
	fseek(f, pos, SEEK_SET);

	return end;
}

/*
** FS_CreatePath
**
** Creates any directories needed to store the given filename
*/
void FS_CreatePath(char *path)
{
	char *ofs;
	
	for(ofs=path+1; *ofs; ofs++)
		if(*ofs=='/') // create the directory
		{
			*ofs=0;
			Sys_Mkdir(path);
			*ofs='/';
		}
}

/*
** FS_FCloseFile
**
** For some reason, other dll's can't just cal fclose()
** on files returned by FS_FOpenFile...
*/
void FS_FCloseFile(FILE *f)
{
	fclose(f);
}

/*
** FS_FOpenFile
**
** Finds the file in the search path.
** returns filesize and an open FILE *
** Used for streaming data out of either a pak file or
** a seperate file.
*/
int FS_FOpenFile(char *filename, FILE **file)
{
	char netpath[MAX_OSPATH];
	searchpath_t *search;
	pack_t *pak;
	int n;

// search through the path, one element at a time
	for(search=fs_searchpaths; search; search=search->next)
	{
	// is the element a pak file?
		if(search->pack)
		{
		// look through all the pak file elements
			pak=search->pack;
			for(n=0; n<pak->numfiles; n++)
				if(!Q_strcasecmp(pak->files[n].name, filename))
				{
				// open a new file on the pakfile
					*file=fopen(pak->filename, "rb");
					if(!*file) Sys_Error("Couldn't reopen %s", pak->filename);
					fseek(*file, pak->files[n].filepos, SEEK_SET);
					return pak->files[n].filelen;
				}
		}
		else
		{		
		// check a file in the directory tree
			sprintf(netpath, "%s/%s", search->filename, filename);

			*file=fopen(netpath, "rb");
			if(!*file) continue;
			
			return FS_filelength(*file);
		}
	}

// file not found
	*file=NULL;
	return -1;
}

#define	MAX_READ 0x10000		// read in blocks of 64k

/*
** FS_ReadFile
**
** Properly handles partial reads
*/
void FS_Read(void *buffer, int len, FILE *f)
{
	int block, remaining, read;
	byte *buf;

	buf=(byte *)buffer;

// read in chunks for progress bar
	remaining=len;
	while(remaining)
	{
		block=remaining;
		if(block>MAX_READ) block=MAX_READ;
		read=fread(buf, 1, block, f);

		if(read<=0)
			Sys_Error("FS_Read: %d bytes read", read);

		remaining-=read;
		buf+=read;
	}
}

/*
** FS_LoadFile
**
** Filename are relative to the quake search path
** a null buffer will just return the file length without loading
** text files would get a terminating zero
*/
int FS_LoadFile(char *path, void **buffer, bool text)
{
	FILE *h;
	byte *buf;
	int len;

// look for it in the filesystem or pack files
	len=FS_FOpenFile(path, &h);
	if(!h)
	{
		if(buffer) *buffer=NULL;
		return -1;
	}

	if(!buffer)
	{
		fclose(h);
		return len;
	}

	if(text)
	{
		buf=Z_Malloc(len+1);
		buf[len]=0;
	}
	else
		buf=Z_Malloc(len);

	*buffer=buf;

	FS_Read(buf, len, h);
	FS_FCloseFile(h);

	if(text)
		return len+1;
	else
		return len;
}

/*
** FS_FreeFile
*/
void FS_FreeFile(void *buffer)
{
	Z_Free(buffer);
}

/*
** FS_LoadPackFile
**
** Takes an explicit (not game tree related) path to a pak file.
**
** Loads the header and directory, adding the files at the beginning
** of the list so they override previous pack files.
*/
pack_t *FS_LoadPackFile(char *packfile)
{
	dpackheader_t header;
	dpackfile_t info[MAX_FILES_IN_PACK];
	packfile_t *newfiles;
	pack_t *pack;
	FILE *packhandle;
	int numpackfiles;
	int n;

	packhandle=fopen(packfile, "rb");
	if(!packhandle) return NULL;

	fread(&header, 1, sizeof(header), packhandle);
	if(header.ident!=IDPAKHEADER)
	{
		Con_Printf("Warning: %s is not a packfile, discarded.\n", packfile);
		return NULL;
	}

	numpackfiles=header.dirlen/sizeof(dpackfile_t);

	if(numpackfiles>MAX_FILES_IN_PACK)
	{
		Con_Printf("Warning: %s has too many (%i) files, discarded.\n", packfile, numpackfiles);
		return NULL;
	}

	newfiles=Z_Malloc(numpackfiles*sizeof(packfile_t));

	fseek(packhandle, header.dirofs, SEEK_SET);
	fread(info, 1, header.dirlen, packhandle);

// parse the directory
	for(n=0; n<numpackfiles; n++)
	{
		strcpy(newfiles[n].name, info[n].name);
		newfiles[n].filepos=info[n].filepos;
		newfiles[n].filelen=info[n].filelen;
	}

	pack=Z_Malloc(sizeof(pack_t));
	strcpy(pack->filename, packfile);
	pack->handle=packhandle;
	pack->numfiles=numpackfiles;
	pack->files=newfiles;
	
	Con_Printf("Added packfile %s (%i files)\n", packfile, numpackfiles);
	return pack;
}

/*
** FS_AddGameDirectory
**
** Sets fs_gamedir, adds the directory to the head of the path,
** then loads and adds pak1.pak pak2.pak ... 
*/
void FS_AddGameDirectory(char *dir)
{
	searchpath_t *search;
	pack_t *pak;
	char pakfile[MAX_OSPATH];
	int n;

	strcpy(fs_gamedir, dir);

// add any pak files in the format pak0.pak pak1.pak, ...
	for(n=0; n<10; n++)
	{
		sprintf(pakfile, "%s/pak%d.pak", dir, n);
		pak=FS_LoadPackFile(pakfile);
		if(!pak) continue;
		search=Z_Malloc(sizeof(searchpath_t));
		search->filename[0]=0;
		search->pack=pak;
		search->next=fs_searchpaths;
		fs_searchpaths=search;		
	}

// add the directory itself to the search path
	search=Z_Malloc(sizeof(searchpath_t));
	strcpy(search->filename, dir);
	search->pack=NULL;
	search->next=fs_searchpaths;
	fs_searchpaths=search;
}

/*
** FS_Gamedir
**
** Called to find where to write a file (demos, savegames, etc)
*/
char *FS_Gamedir(void)
{
	if(*fs_gamedir)
		return fs_gamedir;
	else
		return BASEDIRNAME;
}

/*
** FS_ExpandFilename
**
** prefixes filename with current game directory
*/
char *FS_ExpandFilename(char *name)
{
	static char fullname[MAX_OSPATH];

	sprintf(fullname, "%s/%s", FS_Gamedir(), name);
	return fullname;
}

/*
** FS_SetGamedir
**
** Sets the gamedir and path to a different directory.
*/
void FS_SetGamedir(char *dir)
{
	searchpath_t *next;

	if(strchr(dir, '/') || strchr(dir, '\\'))
	{
		Con_Printf("Gamedir should be a single filename, not a path\n");
		return;
	}

// free up any current game dir info
	while(fs_searchpaths!=fs_base_searchpaths)
	{
		if(fs_searchpaths->pack)
		{
			fclose(fs_searchpaths->pack->handle);
			free(fs_searchpaths->pack->files);
			free(fs_searchpaths->pack);
		}
		next=fs_searchpaths->next;
		free(fs_searchpaths);
		fs_searchpaths=next;
	}

	sprintf(fs_gamedir, "%s/%s", fs_basedir->string, dir);

	if(!strcmp(dir, BASEDIRNAME) || !*dir)
	{
		Cvar_FullSet("gamedir", "", CVAR_SERVERINFO|CVAR_NOSET);
		Cvar_FullSet("game", "", CVAR_SERVERINFO|CVAR_LATCH);
	}
	else
	{
		Cvar_FullSet("gamedir", dir, CVAR_SERVERINFO|CVAR_NOSET);
		FS_AddGameDirectory(va("%s/%s", fs_basedir->string, dir));
	}
}

/*
** FS_Path_f
*/
void FS_Path_f(void)
{
	searchpath_t *s;

	Con_Printf("Current directory: %s\n", FS_ExpandFilename(""));
	Con_Printf("Current search path:\n");
	for(s=fs_searchpaths; s; s=s->next)
	{
		if(s==fs_base_searchpaths)
			Con_Printf("----------\n");
		if(s->pack)
			Con_Printf("%s (%i files)\n", s->pack->filename, s->pack->numfiles);
		else
			Con_Printf("%s\n", s->filename);
	}
}

/*
** FS_InitFilesystem
*/
void FS_InitFilesystem(void)
{
	Cmd_AddCommand("path", FS_Path_f);

// basedir <path>
// allows the game to run from outside the data tree
	fs_basedir=Cvar_Get("basedir", ".", CVAR_NOSET);

// start up with default basedir
	FS_AddGameDirectory(va("%s/"BASEDIRNAME, fs_basedir->string));

// any set gamedirs will be freed up to here
	fs_base_searchpaths=fs_searchpaths;

// check for game override
	fs_gamedirvar=Cvar_Get("game", "", CVAR_LATCH|CVAR_SERVERINFO);
	if(fs_gamedirvar->string[0]) FS_SetGamedir(fs_gamedirvar->string);
}
