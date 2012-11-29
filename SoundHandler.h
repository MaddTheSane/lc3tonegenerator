
#include <dsound.h>

typedef struct LC3TimeTable
{
	short tone;
	short length;
} LC3TimeTable;

class LC3Sound
{
private:
	BOOL LoadSamp(LPDIRECTSOUND lpDirectSound,
              LPDIRECTSOUNDBUFFER *lplpDsb,
              LPBYTE samp, UINT length, UINT flags);
	BOOL WriteDataToBuffer(
    LPDIRECTSOUNDBUFFER lpDsb,
    DWORD dwOffset,
    LPBYTE lpbSoundData,
    DWORD dwSoundBytes);

	void PopTable();

	bool DirectSave( char* sndData);

	BOOL AppCreateWritePrimaryBuffer(
    LPDIRECTSOUND lpDirectSound,
    LPDIRECTSOUNDBUFFER *lplpDsb,
    HWND hwnd);


	LC3TimeTable table[10];
	int timeLeft;
	int tableIndex;
	HWND hwnd;
	UINT gwid;
	char *soundData;
	GUID soundGUID;
	LPDIRECTSOUND			lpDirectSound;									// The LPDIRECTSOUND to apply & get informations, etc.
	LPDIRECTSOUNDBUFFER		lpDirectSoundBuffer, lpSwSamp;
public:
	LC3Sound();
	virtual ~LC3Sound();
	int AddTimeTable(LC3TimeTable theTable);
	void TimeProc(
	 UINT  IDEvent,			/* identifies timer event */
	 UINT  uReserved,		/* not used */
	 DWORD_PTR  dwUser,			/* application-defined instance data */
	 DWORD_PTR  dwReserved1,	/* not used */
	 DWORD_PTR  dwReserved2		/* not used */
);
};

//bool InitSoundHandler();
