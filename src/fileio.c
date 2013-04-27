// ------------------------- * File I/O * -------------------------
/* WolfGL main file IO code!
================================
	My own code + some code ripped from wolf's src + some from
	WolfEx by Bruce Lewis
================================
*/
#include <windows.h>
#include <stdio.h>
#include "WolfDef.h"

typedef struct
{
	unsigned short bit0, bit1;
} huffnode;

typedef struct
{
	unsigned int bit0, bit1; // 0-255 is a character, > is a pointer to a node
} huffnode32;

typedef	struct
{
	unsigned long offset; // Offset of chunk into file
	unsigned long length; // Length of the chunk
}	chunk;

void File_COM_ReadChunk(FILE *fp, unsigned char *data, chunk *ch);
void File_OptimizeNodes(huffnode *table, huffnode32 *table2);
void File_HuffExpand(unsigned char *source, unsigned char *dest, long length, long c_len, huffnode32 *hufftable);

typedef struct
{
	int width;
	int height;
	char is_alpha;
	int mem; // amount of memory to store that gfx
} gfx;

typedef struct
{
	unsigned short leftpix, rightpix;
	unsigned short dataofs[64];
}	shape_t;

typedef struct
{
	unsigned short width, height;
} wl_picture;
// ------------------------- * Devider * -------------------------
typedef struct
{
	unsigned short ChunksInFile;
	unsigned short SpriteStart;
	unsigned short SoundStart;
	chunk *Pages;
	char FileName[MAX_OSPATH];
}	PF_Struct;

typedef struct
{
	int TotalMaps;
	char HeadName[MAX_OSPATH];
	char FileName[MAX_OSPATH];
	long *offset;
} MF_Struct;

typedef struct
{
	int TotalChunks;
	char HeadName[MAX_OSPATH];
	char DictName[MAX_OSPATH];
	char FileName[MAX_OSPATH];
	long *offset;
	wl_picture *pictable;
	huffnode32 grhuffman32[256];
} VF_Struct;

typedef struct
{
	int TotalChunks;
	char HeadName[MAX_OSPATH];
	char FileName[MAX_OSPATH];
	long *offset;
} AF_Struct;

PF_Struct PageFile;
MF_Struct MapFiles;
VF_Struct VgaFiles;
AF_Struct AudFiles;

#define PROJECT_DOOR_COUNT 8

// float fVer=1.1f; // Only 1.1+ is supported
// ------------------------- * Start-End * -------------------------
int File_Init(void)
{
	memset(&PageFile, 0, sizeof(PageFile));
	memset(&MapFiles, 0, sizeof(MapFiles));
	memset(&VgaFiles, 0, sizeof(VgaFiles));
	memset(&AudFiles, 0, sizeof(AudFiles));

	//File_LoadPalette(); //FIXME: palette is initialized in version.
	File_PML_OpenPageFile(WolfVer.file_page);
	File_VGA_OpenVgaFiles(WolfVer.file_vgah, WolfVer.file_vgad, WolfVer.file_vgam);
	File_AUD_OpenAudioFiles(WolfVer.file_audh, WolfVer.file_audm);
	Cmd_AddCommand("dir", File_Dir_f);
	Cmd_AddCommand("crc", File_CRC_f);
	Con_Printf("File_Init: Done!\n");
	return 1;
}

void File_ShutDown(void)
{
	if(PageFile.Pages) free(PageFile.Pages);
	if(MapFiles.offset) free(MapFiles.offset);
	if(VgaFiles.offset) free(VgaFiles.offset);
	if(VgaFiles.pictable) free(VgaFiles.pictable);
}

// ------------------------- * VSWAP (PageFile) * -------------------------

// Opens the page file and sets up the pages info
int File_PML_OpenPageFile(char *filename)
{
	unsigned short tmp;
	int i;
	FILE *fp;

	fp=fopen(filename, "rb");
	if(!fp)
	{
		Con_Printf("FileIO: Unable to open page file: %s\n", filename);
		return 0;
	}

	if(PageFile.Pages) free(PageFile.Pages);
	strcpy(PageFile.FileName, filename);

	// Read in header variables
	fread(&PageFile.ChunksInFile, sizeof(unsigned short), 1, fp);
	fread(&PageFile.SpriteStart,	sizeof(unsigned short), 1, fp);
	fread(&PageFile.SoundStart,		sizeof(unsigned short), 1, fp);

	Con_Printf("FileIO: Page File:\n");
	Con_Printf("-> main: %s\n", filename);
  Con_Printf("-> Total Chunks : %d\n", PageFile.ChunksInFile);
  Con_Printf("-> Sprites start: %d\n", PageFile.SpriteStart);
  Con_Printf("-> Sounds start : %d\n", PageFile.SoundStart);

	// Allocate and clear the page list -- actually a chunk list
  PageFile.Pages=(chunk *)calloc(PageFile.ChunksInFile, sizeof(chunk));

	// Read in the chunk offsets
	for(i=0; i<PageFile.ChunksInFile; i++)
		fread(&PageFile.Pages[i].offset, sizeof(long), 1, fp);

	// Read in the chunk lengths
  for(i=0; i<PageFile.ChunksInFile; i++)
	{
		fread(&tmp, sizeof(unsigned short), 1, fp);
		PageFile.Pages[i].length=tmp;
	}

	fclose(fp);
	return 1;
}

int File_PML_ReadPage(int n, unsigned char *data)
{
	FILE *fp;

	if(!PageFile.FileName[0])
	{
		Con_Printf("FileIO: Page file not opened\n");
		return 0;
	}
	if(n>=PageFile.ChunksInFile)
	{
		Con_Printf("FileIO: Wrong chunk num %d\n", n);
		return 0;
	}
	if(!PageFile.Pages[n].length || !PageFile.Pages[n].offset)
	{
		Con_Printf("FileIO: Page %d wrong header data\n", n);
		return 0;
	}
	if(!data)
	{
		Con_Printf("FileIO: Bad Pointer!\n"); 
		return 0;
	}

	fp=fopen(PageFile.FileName, "rb");
	if(!fp)
	{
		Con_Printf("FileIO: Unable to open page file\n");
		return 0;
	}
	fseek(fp, PageFile.Pages[n].offset, SEEK_SET);
	if(fread(data, 1, PageFile.Pages[n].length, fp)!=PageFile.Pages[n].length)
	{
		Con_Printf("FileIO: Page %d read error\n", n);
		return 0;
	}
	fclose(fp);
	return 1;
}

// ------------------------- * Load Wall Textures * -------------------------
int File_PML_LoadWall(int n, unsigned char *block)
{
	int x, y;
	unsigned char *data;

	if(n>=PageFile.SpriteStart)
	{
		Con_Printf("FileIO: Wall index (%d) out of bounds [0-%d]\n", n, PageFile.SpriteStart);
		return 0;
	}
	data=malloc(PageFile.Pages[n].length);
	if(!File_PML_ReadPage(n, data)) return 0;

// ------------------------- * Making data more usefull for me! * -------------------------
	for(x=0; x<64; x++)
	{
		for(y=0; y<64; y++)
		{
			block[((y<<6)+x)*3+0] = WolfPal[data[(x<<6)+y]].r;
			block[((y<<6)+x)*3+1] = WolfPal[data[(x<<6)+y]].g;
			block[((y<<6)+x)*3+2] = WolfPal[data[(x<<6)+y]].b;
		}
	}
	free(data);
	return 1;
}

int File_PML_LoadDoor(int n, unsigned char *block)
{
	return File_PML_LoadWall(n+(PageFile.SpriteStart-PROJECT_DOOR_COUNT), block);
}

int File_PML_LoadSprite(int n, unsigned char *block)
{
  int i, x, y;
  unsigned char *sprite, tmp[64*64];
  unsigned short *cmdptr;
  short *linecmds;
  shape_t *shape;

	if(n<0 || n>PageFile.SoundStart-PageFile.SpriteStart)
	{
		Con_Printf("FileIO: Sprite index (%d) out of bounds [0-%d]\n", n, PageFile.SoundStart-PageFile.SpriteStart);
		return 0;
	}
	n+=PageFile.SpriteStart;

	sprite=malloc(PageFile.Pages[n].length);
	if(!File_PML_ReadPage(n, sprite)) return 0;

// all transparent at the beginning
	memset(tmp, 255, sizeof(tmp));

// Very weird code by Mr. Bruce Lewis (btw creator of DoomGL) need to be rewritten!
	shape=(shape_t *)sprite;

	cmdptr=shape->dataofs;
	for(x=shape->leftpix; x<=shape->rightpix; x++)
  {
    linecmds=(short *)(sprite+*cmdptr++);
    for(; *linecmds; linecmds+=3)
    {
			i=linecmds[2]/2+linecmds[1];
			for(y=linecmds[2]/2; y<linecmds[0]/2; y++, i++)
				tmp[y*64+x]=sprite[i];
    }
  }
	Img_ExpandPalette(block, tmp, 64, 64, (byte *)WolfPal, true);

	free(sprite);
	return 1;
}

int File_PML_LoadDigiMap(void **buff)
{
	*buff=malloc(PageFile.Pages[PageFile.ChunksInFile-1].length);
	if(!File_PML_ReadPage(PageFile.ChunksInFile-1, *buff))
	{
		free(*buff);
		return -1;
	}
	return PageFile.Pages[PageFile.ChunksInFile-1].length;
}

int File_PML_LoadSound(int st, int len, unsigned char *sndbuff)
{
	int n, slen=0;

	for(n=st; slen<len; n++)
	{
		if(!File_PML_ReadPage(PageFile.SoundStart+n, &sndbuff[slen]))
		{
			Con_Printf("FileIO: Error Loading Sound\n");
			return 0;
		}
		slen+=PageFile.Pages[PageFile.SoundStart+n].length;
	}
	return slen;
}
// ------------------------- * VGA-Graphics * -------------------------
int File_VGA_OpenVgaFiles(char *headername, char *dictname, char *vganame)
{
	FILE *fp;
	long size;
	int n;
	unsigned char temp[3];
	huffnode hufftmp[256];
	unsigned char *picdef;

	if(!File_COM_Exist(dictname))
	{	Con_Printf("FileIO: graphics dictionary missed: %s\n", dictname);	return 0;	}
	if( (size=( File_COM_Size(headername) )) ==-1 )
	{	Con_Printf("FileIO: graphics header missed: %s\n", headername);	return 0;	}
	if(!File_COM_Exist(vganame))
	{	Con_Printf("FileIO: VGA graphics file missed: %s\n", vganame);	return 0;	}
	
	if(VgaFiles.offset) free(VgaFiles.offset);
	if(VgaFiles.pictable) free(VgaFiles.pictable);
	memset(&VgaFiles, 0, sizeof(VgaFiles));

	fp=fopen(dictname, "rb");
	if(fread(hufftmp, 1, 1024, fp)!=1024)
	{
		Con_Printf("FileIO: Wrong VGA dictionary: %s\n", dictname);
		fclose(fp);
		return 0;
	}
	fclose(fp);
	File_OptimizeNodes(hufftmp, VgaFiles.grhuffman32);

	VgaFiles.TotalChunks=size/3;
	VgaFiles.offset=malloc(VgaFiles.TotalChunks*4);

	fp=fopen(headername, "rb");
	for(n=0; n<VgaFiles.TotalChunks; n++)
	{
		fread(temp, 1, 3, fp);
		VgaFiles.offset[n]=temp[0]+(temp[1]<<8)+(temp[2]<<16);
		if(VgaFiles.offset[n]==0xFFFFFF) VgaFiles.offset[n]=-1;
	}
	fclose(fp);
	
	strcpy(VgaFiles.DictName, dictname);
	strcpy(VgaFiles.HeadName, headername);
	strcpy(VgaFiles.FileName, vganame);

	picdef=malloc(VgaFiles.TotalChunks*4);
	VgaFiles.pictable=(wl_picture*)picdef;
	File_VGA_ReadChunk(0, picdef); // read picture defines!

	Con_Printf("FileIO: VGA graphics files:\n");
	Con_Printf("-> dict: %s\n", dictname);
	Con_Printf("-> head: %s\n", headername);
	Con_Printf("-> main: %s\n", vganame);
	Con_Printf("-> Total Chunks: %d\n", VgaFiles.TotalChunks);
	return 1;
}

long File_VGA_GetChunkSize(int n)
{
	FILE *fp;
	long size;

	if(n<0 || n>VgaFiles.TotalChunks)
	{
		Con_Printf("FileIO: VGA chunk index out of bounds [0, %d]: %d\n", VgaFiles.TotalChunks, n);
		return 0;
	}
	fp=fopen(VgaFiles.FileName, "rb");
	if(fp==NULL) return 0;
	fseek(fp, VgaFiles.offset[n], SEEK_SET);
	if(fread(&size, 4, 1, fp)!=1)
	{
		fclose(fp);
		return 0;
	}
	fclose(fp);
	return size;
}

int File_VGA_ReadChunk(int n, unsigned char *target)
{
	FILE *fp;
	long pos, compressed, expanded;
	unsigned char *src;
	unsigned char *source;
	int next;

	if(n<0 || n>VgaFiles.TotalChunks)
	{
		Con_Printf("FileIO: VGA chunk index out of bounds [0, %d]: %d\n", VgaFiles.TotalChunks, n);
		return 0;
	}
	fp=fopen(VgaFiles.FileName, "rb");
	if(fp==NULL) return 0;
	// load the chunk into a buffer

	pos=VgaFiles.offset[n];
	next=n+1;
	while(VgaFiles.offset[next]==-1) next++; // skip past any sparse tiles
	compressed=VgaFiles.offset[next]-pos;
	src=malloc(compressed);

	fseek(fp, pos, SEEK_SET);
	fread(src, 1, compressed, fp);
	fclose(fp);

	source=src;
	if(n==0) // treat zero chunk separately, why? Magic! Damn Magic!
		expanded=VgaFiles.TotalChunks*4;
	else
		expanded= *(long *)src;
	source+=4;			// skip over length

	if(expanded==0)	{free(src); return 0;}
	
	File_HuffExpand(source, target, expanded, compressed, VgaFiles.grhuffman32);
	free(src);

	return 1;
}

int File_VGA_ReadPic(int chunk, wl_pic *pic)
{
	int	picnum;
	int size, width, height, quarter, hw;
	unsigned char *buf, *buf1;
	int n;

	picnum=chunk-3; // STARTPICS (0..2 are special chunks!) FIXME
	if(picnum<0) return 0;

	size=File_VGA_GetChunkSize(chunk);
	if(size<=0) return 0;
	
	width =VgaFiles.pictable[picnum].width;
	height=VgaFiles.pictable[picnum].height;
	if(width<1 || width>320 || height<1 || height>200) return 0; // not a picture

	buf=malloc(size);
	if(!File_VGA_ReadChunk(chunk, buf))	{free(buf); return 0;}
	buf1=malloc(size);
	
	width =VgaFiles.pictable[picnum].width;
	height=VgaFiles.pictable[picnum].height;
	hw=width*height;
	quarter=hw/4;

	for(n=0; n<hw; n++)
		buf1[n]=buf[(n%4)*quarter+n/4];

	pic->width=width;
	pic->height=height;
	pic->data=malloc(hw*3);
	for(n=0; n<hw; n++)
	{
		pic->data[n*3+0]=WolfPal[buf1[n]].r;
		pic->data[n*3+1]=WolfPal[buf1[n]].g;
		pic->data[n*3+2]=WolfPal[buf1[n]].b;
	}
	free(buf);
	free(buf1);
	return 1;
}
// ------------------------- * Maps * -------------------------
int File_MAP_OpenMapFiles(char *filename, char *mapfile)
{
	FILE *fp;
	long size;
	unsigned short sig;
	unsigned long data;
	int n;

	size=File_COM_Size(filename);
	if(size<6)
	{
		Con_Printf("FileIO: Wrong map header file: %s\n", filename);
		return 0;
	}
	if(!File_COM_Exist(mapfile))
	{
		Con_Printf("FileIO: Map file not found: %s\n", mapfile);
		return 0;
	}
	size=(size-2)/4;
	
	fp=fopen(filename, "rb");
	if(fp==NULL)
	{
		Con_Printf("FileIO: Can't open map header: %s\n", filename);
		return 0;
	}

	fread(&sig, 2, 1, fp);
	if(sig!=0xABCD)
	{
		Con_Printf("FileIO: Wrong map header file: %s\n", filename);
		fclose(fp);
		return 0;
	}

  for(n=0; n<size; n++)
	{
		fread(&data, 4, 1, fp);
		if(!data)	break;
	}
	Con_Printf("FileIO: Map Files:\n");
	Con_Printf("-> head: %s\n", filename);
	Con_Printf("-> main: %s\n", mapfile);
  Con_Printf("-> Total Levels: %d\n", n);

	if(MapFiles.offset) free(MapFiles.offset);

	strcpy(MapFiles.FileName, mapfile);
	strcpy(MapFiles.HeadName, filename);
	MapFiles.TotalMaps=n;

	MapFiles.offset=malloc(n*4);
	fseek(fp, 2, SEEK_SET);
	for(n=0; n<MapFiles.TotalMaps; n++)
		fread(&MapFiles.offset[n], 4, 1, fp);

	fclose(fp);
	return n;	
}

int File_MAP_ReadData(int level, unsigned short *layer1, unsigned short *layer2, unsigned short *layer3, char *name)
{
	chunk l1={0, 0}, l2={0, 0}, l3={0, 0};
	unsigned char *data, idsig[5];
	unsigned short w, h;
	FILE *fp;
	
	if(level>=MapFiles.TotalMaps || level<0)
	{
		Con_Printf("FileIO: Level number out of bounds [%d, %d]\n", 0, MapFiles.TotalMaps-1);
		return 0;
	}

	fp=fopen(MapFiles.FileName, "rb");
	if(fp==NULL) return 0;
	fseek(fp, MapFiles.offset[level], SEEK_SET);
	fread(&l1.offset, 4, 1, fp);
	fread(&l2.offset, 4, 1, fp);
	fread(&l3.offset, 4, 1, fp);
	fread(&l1.length, 2, 1, fp);
	fread(&l2.length, 2, 1, fp);
	fread(&l3.length, 2, 1, fp);
	fread(&w, 2, 1, fp);
	fread(&h, 2, 1, fp);

	if(w!=64 || h!=64)
	{
		Con_Printf("FileIO: Map %d is not 64x64 (%dx%d)\n", level, w, h);
		fclose(fp);
		return 0;
	}

	fread(name, 1, 16, fp);	name[16]='\0';
	fread(idsig, 1, 4, fp);	idsig[4]='\0';

	data=malloc(l1.length);
	File_COM_ReadChunk(fp, data, &l1);
	File_MAP_Expand((unsigned short *)data, layer1);

	data=realloc(data, l2.length);
	File_COM_ReadChunk(fp, data, &l2);
	File_MAP_Expand((unsigned short *)data, layer2);

	data=realloc(data, l3.length);
	File_COM_ReadChunk(fp, data, &l3);
	File_MAP_Expand((unsigned short *)data, layer3);

	free(data);
	fclose(fp);
	return 1;
}

void File_MAP_Expand(word *source, word *dest)
{
	int x=0, y=0;
	word *buffer2, expanded;

	//
	// unhuffman, then unRLEW
	// The huffman'd chunk has a two byte expanded length first
	// The resulting RLEW chunk also does, even though it's not really
	// needed
	//
	expanded=*source;
	source++;
	buffer2=malloc(expanded);
	File_CarmackExpand(source, buffer2, expanded);
	File_RLEWexpand(buffer2+1, dest, 64*64*2, 0xABCD);
	free(buffer2);
}

// ------------------------- * Audio * -------------------------
int File_AUD_OpenAudioFiles(char *filename, char *audfile)
{
	FILE *fp;
	long size;

	size=File_COM_Size(filename);
	if(size<4)
	{
		Con_Printf("FileIO: Wrong audio header file: %s\n", filename);
		return 0;
	}
	if(!File_COM_Exist(audfile))
	{
		Con_Printf("FileIO: Audio file not found: %s\n", audfile);
		return 0;
	}
	size/=4; // each offset is long (4 bytes)

	fp=fopen(filename, "rb");
	if(fp==NULL)
	{
		Con_Printf("FileIO: Can't open audio header: %s\n", filename);
		return 0;
	}

	if(AudFiles.offset) free(AudFiles.offset);
	memset(&AudFiles, 0, sizeof(AudFiles));

	AudFiles.offset=malloc(size*4);
	fread(AudFiles.offset, 4, size, fp);

	strcpy(AudFiles.FileName, audfile);
	strcpy(AudFiles.HeadName, filename);
	AudFiles.TotalChunks=size;

	Con_Printf("FileIO: Audio Files:\n");
	Con_Printf("-> head: %s\n", AudFiles.HeadName);
	Con_Printf("-> main: %s\n", AudFiles.FileName);
  Con_Printf("-> Total Chunks: %d\n", AudFiles.TotalChunks);

	fclose(fp);
	return size;
}

// returns length of chunk, 0 if error
int File_AUD_ReadChunk(int n, unsigned char *buff)
{
	FILE *fp;
	long pos, len;

	if(n<0 || n>=AudFiles.TotalChunks)
	{
		Con_Printf("FileIO: VGA chunk index out of bounds [0, %d]: %d\n", AudFiles.TotalChunks, n);
		return 0;
	}

	pos=AudFiles.offset[n];
	len=AudFiles.offset[n+1]-pos;
	if(len<=0) return 0;

	fp=fopen(AudFiles.FileName, "rb");
	if(fp==NULL) return 0;
	fseek(fp, pos, SEEK_SET);
//	*buff=malloc(len); // **buff in param!
	fread(buff, 1, len, fp); // *buff
	fclose(fp);
	return len;
}
// ------------------------- * Compression-Expansion * -------------------------
/*
======================
= File_CarmackExpand, Length is the length of the EXPANDED data in !bytes!
======================
*/
#define NEARTAG	0xA7
#define FARTAG	0xA8
void File_CarmackExpand(word *source, word *dest, word length)
{
	word ch, chhigh, count, offset;
	word *copyptr, *inptr, *outptr;

	length/=2; inptr=source; outptr=dest;

	while(length)
	{
		ch=*inptr++;
		chhigh=ch>>8;
		if(chhigh==NEARTAG)
		{
			count=ch&0xff;
			if(!count)
			{// have to insert a word containing the tag byte
				ch|= *((unsigned char *)inptr)++;
				*outptr++ =ch;
				length--;
			}
			else
			{
				offset= *((unsigned char *)inptr)++;
				copyptr=outptr-offset;
				length-=count;
				while(count--) *outptr++ = *copyptr++;
			}
		}
		else if(chhigh==FARTAG)
		{
			count=ch&0xff;
			if (!count)
			{// have to insert a word containing the tag byte
				ch|= *((unsigned char *)inptr)++;
				*outptr++ =ch;
				length--;
			}
			else
			{
				offset= *inptr++;
				copyptr=dest+offset;
				length-=count;
				while(count--) *outptr++ = *copyptr++;
			}
		}
		else
		{
			*outptr++ =ch;
			length--;
		}
	}
}

void File_RLEWexpand(word *source, word *dest, long length, word rlewtag)
{
	word *end;
	word value, count;
	int n;

	end=dest+(length)/2;
	do
	{
		value=*source++;
		if(value!=rlewtag)	// uncompressed
			*dest++ =value;
		else // compressed string
		{
			count=*source++;
			value=*source++;
			for(n=1; n<=count; n++)	*dest++ =value;
		}
	} while(dest<end);
}

/*
===============
= CAL_OptimizeNodes
=
= Goes through a huffman table and changes the 256-511 node numbers to the
= actual address of the node.  Must be called before CAL_HuffExpand
===============
*/
void File_OptimizeNodes(huffnode *table, huffnode32 *table2)
{
	int i;

	for(i=0; i<255; i++)
	{
		if(table[i].bit0>=256)
			table2[i].bit0=(unsigned)(table2+(table[i].bit0-256));
		else
			table2[i].bit0=table[i].bit0;
		if(table[i].bit1>=256)
			table2[i].bit1=(unsigned)(table2+(table[i].bit1-256));
		else
			table2[i].bit1=table[i].bit1;
	}
}

// File_HuffExpand
//
// Length is the length of the EXPANDED data
// c_len is length of compressed, to avoid invalide page faults!
void File_HuffExpand(unsigned char *source, unsigned char *dest, long length, long c_len, huffnode32 *hufftable)
{
	huffnode32 *headptr, *node;
	unsigned char *end, ch, cl;

	headptr=&hufftable[254]; // head node is allways node 254
// For everybody who uses 'normalize' in 32-bit programms: it is only for huge pointers!
	end=dest+length;

	node=headptr;
	ch=*source;
	source++;
	c_len--;
	cl=1;

	do
	{
		if(ch&cl)
			node=(huffnode32*)node->bit1;
		else
			node=(huffnode32*)node->bit0;

		if((int)node<256)
		{
			*dest=(unsigned char)((int)node);
			dest++;
			node=headptr;
		}

		cl<<=1;

		if(cl==0)
		{
			c_len--;
			if(c_len<0) return; // no more data to read!

			ch=*source;
			source++;
			cl=1;
		}
	} while(dest!=end);
}

// ------------------------- * Common File System Functions * -------------------------
// Returns 1 if file exists, or 0 if not
int File_COM_Exist(char *filename)
{
	FILE *fp;

	fp=fopen(filename, "rb");
	if(fp==NULL) return 0;
	fclose(fp);
	return 1;
}

// Returns length of file in bytes, or -1 if no file exists
long File_COM_Size(char *filename)
{
	FILE *fp;
	long pos=-1;

	fp=fopen(filename, "rb");
	if(fp==NULL) return -1;
	fseek(fp, 0, SEEK_END);
	pos=ftell(fp);
	fclose(fp);
	return pos;
}

/*
================
COM_filelength
================
*/
int File_COM_filelength(FILE *fp)
{
	int	pos;
	int	end;

	pos=ftell(fp);
	fseek(fp, 0, SEEK_END);
	end=ftell(fp);
	fseek(fp, pos, SEEK_SET);

	return end;
}

// opens file for binary read & retruns it's length in bytes
// return -1 if error
int File_COM_FileOpenRead(char *path, FILE **hndl)
{
	FILE *fp;

	fp=fopen(path, "rb");
	if(fp==NULL)
	{
		*hndl=NULL;
		return -1;
	}
	*hndl=fp;
	return File_COM_filelength(fp);
}

// reads chunk from file to data
void File_COM_ReadChunk(FILE *fp, unsigned char *data, chunk *ch)
{
	fseek(fp, ch->offset, SEEK_SET);
	fread(data, 1, ch->length, fp);
}

// returns CRC-16 of specified file or -1 if error!
int File_COM_CRCfile(char *path)
{
	FILE *fp;
	unsigned char *data;
	int len, crc;

	len=File_COM_FileOpenRead(path, &fp);
	if(len<0) return -1;
	data=malloc(len);
	if(!data) {fclose(fp); return -1;}
	fread(data, 1, len, fp);
	fclose(fp);
	crc=CRC_Block(data, len);
	free(data);
	return crc;
}

// ------------------------- * TGA file IO * -------------------------

#pragma pack(push, 1)
typedef struct
{
	unsigned char  id_length, colormap_type, image_type;
	unsigned short colormap_index, colormap_length;
	unsigned char  colormap_size;
	unsigned short x_origin, y_origin, width, height;
	unsigned char  pixel_size, attributes;
} TargaHeader;
#pragma pack(pop)

/*
** File_TGA_Write
**
** writes uncompressed TGA file
*/
void File_TGA_Write(char *filename, texture_t *tex)
{
	TargaHeader header;
	int n, tmp, size;
	char *p;
	FILE *fp;

	fp=fopen(filename, "wb");
	if(fp==NULL)
	{
		Con_Printf("FileIO: Error opening %s: %s\n", filename, strerror(errno));
		return; // file not found
	}

	memset(&header, 0, sizeof(header));
	header.image_type=2; // uncompressed
	header.width=tex->width;
	header.height=tex->height;
	header.pixel_size=tex->bpp*8;
	size=tex->width*tex->height;

// swap RGB to BGR
	for(n=0, p=tex->data; n<size; n++, p+=tex->bpp)
	{
		tmp=p[0]; p[0]=p[2]; p[2]=tmp;
	}
	fwrite(&header, 1, sizeof(header), fp);
	fwrite(tex->data, 1, size*tex->bpp, fp);
	fclose(fp);
}

// ------------------------- * Devider * -------------------------

// prints Dir to console... (same as DOS dir command!)
void File_Dir_f(void)
{
	char *name;
	HANDLE dir;
	WIN32_FIND_DATA dir_data;

	if(Cmd_Argc()==1)
		name=FS_ExpandFilename("*.*");
	else
		name=FS_ExpandFilename(Cmd_Argv(1));
	
	dir=FindFirstFile(name, &dir_data);
	if(dir==INVALID_HANDLE_VALUE)
	{
		Con_Printf("Error, wrong param: %s\n", name);
		return;
	}
	Con_Printf("Directory of %s:\n", name);
	do
	{
	// exclude . & ..
		if(dir_data.cFileName[0]=='.')
		{
			if(dir_data.cFileName[1]==0 || (dir_data.cFileName[1]=='.' && dir_data.cFileName[2]==0))
				continue;
		}
		if(dir_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			Con_Printf("%-25s <DIR>\n", dir_data.cFileName);
		else
			Con_Printf("%-25s %11d\n", dir_data.cFileName, dir_data.nFileSizeLow);
	} while(FindNextFile(dir, &dir_data));
	FindClose(dir);
}

// writes file's CRC to console!
void File_CRC_f(void)
{
	if(Cmd_Argc()!=2)
		Con_Printf("crc <filename>: calculate CRC-16 CCIT\n");
	else
		Con_Printf("CRC-16 CCIT of %s: %d\n", Cmd_Argv(1), File_COM_CRCfile(FS_ExpandFilename(Cmd_Argv(1))));
}

// ------------------------- * Common GFX Loader * -------------------------

/*
** File_LoadTexture
**
** allocates buffer in tex->data and loads texture defined by tex->name
** returns 1 if Ok, 0 if not.
*/
bool File_LoadTexture(texture_t *tex)
{
	int name;

	name=tex->name;

	if(name&TEX_SPRITE_BIT) // sprite
	{
		if(Img_Read(va("textures/wolf/sprt%04d", name&TEX_SPRITE_MASK), (void *)tex))
			return true; // substitution loaded
		name=SPR_REMAP[name&TEX_SPRITE_MASK];
		tex->data=malloc(64*64*4);
		if(!File_PML_LoadSprite(name, tex->data))
			{free(tex->data); return false;}
		tex->width=tex->height=64;
		tex->bpp=4;
	}
	else if(name&TEX_DOOR_BIT) // door
	{
		if(Img_Read(va("textures/wolf/door%04d",name&TEX_DOOR_MASK), (void *)tex))
			return true; // substitution loaded
		name=name&TEX_DOOR_MASK;
		tex->data=malloc(64*64*3);
		if(!File_PML_LoadDoor(name, tex->data))
			{free(tex->data); return false;}
		tex->width=tex->height=64;
		tex->bpp=3;
	}
	else // wall
	{
		if(Img_Read(va("textures/wolf/wall%04d", name), (void *)tex))
			return true; // substitution loaded
		tex->data=malloc(64*64*3);
		if(!File_PML_LoadWall(name, tex->data))
			{free(tex->data); return false;}
		tex->width=tex->height=64;
		tex->bpp=3;
	}
	return true;
}

/*int File_GfxInfo(char *text, gfx buff)
{
	int n, l, ext=0, arst=0, argc=0;
	char fname[256], ext[5];
	char tmp[64];
	int args[5];

	l=strlen(text);
	
	for(n=0; n<l; n++)
	{
		if(text[n]==':')
		{
			if(argst) return 0; // 2 x ':' is an error!
			if(!ext) return 0; // filename without ext!
			ext[n-ext]='\0';
			argst=n+1;
			continue;
		}
		if(text[n]='.')
		{
			if(n==0) return 0; // ext without file!
			if(argst) return 0; // . after argst! (float not allowed)
			if(ext) return 0; // two ext!
			fname[n]='\0';
			ext=n+1;
			continue;
		}
		if(text[n]=',')
		{
			if(!argst) return 0;
			if(argst-n>63) return 0;
			if(
		}
		if(argst)
		{
			
		}
		if(ext)
		{
			if(n-ext>3) return 0; // invalid ext
			ext[n-ext]=text[n];
		}
		fname[n]=text[n];
	}

	while(*p && *p!='\n')
	{
		if(*p==':') break;
		if(*p=='
		p++;
	}
	if(!*p || *p=='\n') return 0;
	if(p-text>255) return 0;
	memcpy(fname, text, p-text);
}*/
