// TestProgram.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SoundHandler.h"

int _tmain(int argc, _TCHAR* argv[])
{
	LC3Sound *sound = new LC3Sound();

	LC3TimeTable table;

	table.length = 100;
	table.tone = 16000;

	sound->AddTimeTable(table);

	table.length = 260;
	table.tone = 2500;

	sound->AddTimeTable(table);

	Sleep(1000);


	delete sound;
	return 0;
}

