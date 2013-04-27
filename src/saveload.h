#define SAVE_SLOTS 10
extern char m_filenames[SAVE_SLOTS][33];
extern bool loadable[SAVE_SLOTS];

void ScanSaves(void);
void InitSaveLoad(void);

bool Save(int slot);
bool Load(int slot);