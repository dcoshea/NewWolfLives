typedef struct
{
	short width;
	short height;
	unsigned char *data;
} wl_pic;

int File_Init(void);
void File_ShutDown(void);

int File_PML_LoadWall(int n, unsigned char *block);
int File_PML_LoadDoor(int n, unsigned char *block);
int File_PML_LoadSprite(int n, unsigned char *block);
int File_PML_LoadDigiMap(void **buff);
int File_PML_LoadSound(int st, int len, unsigned char *sndbuff);

int File_COM_Exist(char *filename);
long File_COM_Size(char *filename);
int File_COM_filelength(FILE *fp);
int File_COM_FileOpenRead(char *path, FILE **hndl);
int File_COM_CRCfile(char *path);

void File_TGA_Write(char *filename, texture_t *tex);

int File_PML_OpenPageFile(char *filename);
int File_PML_ReadPage(int n, unsigned char *data);

int File_VGA_OpenVgaFiles(char *headername, char *dictname, char *vganame);;
long File_VGA_GetChunkSize(int n);
int File_VGA_ReadChunk(int n, unsigned char *target);
int File_VGA_ReadPic(int chunk, wl_pic *pic);

int File_MAP_OpenMapFiles(char *filename, char *mapfile);
int File_MAP_ReadData(int level, unsigned short *layer1, unsigned short *layer2, unsigned short *layer3, char *name);
void File_MAP_Expand(word *source, word *dest);

int File_AUD_OpenAudioFiles(char *filename, char *audfile);
int File_AUD_ReadChunk(int n, unsigned char *buff);

void File_CarmackExpand(word *source, word *dest, word length);
void File_RLEWexpand(word *source, word *dest, long length, word rlewtag);

void File_Dir_f(void);
void File_CRC_f(void);

bool File_LoadTexture(texture_t *tex);
