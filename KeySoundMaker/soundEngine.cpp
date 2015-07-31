# include <stdio.h>
# include <conio.h>
# include <iostream>
# include <fstream>
# include <dirent.h>
# include <vector>
# include <cstring>
# include <time.h>
# include <windows.h>
# include <winuser.h>
# include <mmsystem.h>
# include "soundEngine.h"


SoundEngine::SoundEngine(char* dirStr) {
	srand(time(NULL));
	_dirStr = dirStr;

	DIR *dir;
	char *fileStr;
	char *wav;
	struct dirent *ent;
	if ((dir = opendir(dirStr)) != NULL) {
		while ((ent = readdir(dir)) != NULL) {

			if (strstr(ent->d_name, ".wav")) {
				fileStr = (char*)malloc(strlen(_dirStr) + strlen(ent->d_name) + 2);
				strcpy(fileStr, _dirStr);
				strcpy(&fileStr[strlen(_dirStr)], "\\");
				strcpy(&fileStr[strlen(_dirStr)+1], ent->d_name);

				wav = readWavFile(fileStr);

				if (strstr(ent->d_name, "_down")) {
					if (strstr(ent->d_name, "_space")) {
						downSpaceSounds.push_back(wav);
					}
					else if (strstr(ent->d_name, "_return")) {
						downReturnSounds.push_back(wav);
					}
					else {
						downSounds.push_back(wav);
					}
				}
				else if (strstr(ent->d_name, "_up")) {
					if (strstr(ent->d_name, "_space")) {
						upSpaceSounds.push_back(wav);
					}
					else if (strstr(ent->d_name, "_return")) {
						upReturnSounds.push_back(wav);
					}
					else {
						upSounds.push_back(wav);
					}
				}
			}
		}
		closedir(dir);
	}

	wav = readWavFile("C:\\Windows\\media\\chimes.wav");

	if (downSounds.empty())
		downSounds.push_back(wav);
	if (downSpaceSounds.empty())
		downSpaceSounds.push_back(wav);
	if (downReturnSounds.empty())
		downReturnSounds.push_back(wav);
	if (upSounds.empty())
		upSounds.push_back(wav);
	if (upSpaceSounds.empty())
		upSpaceSounds.push_back(wav);
	if (upReturnSounds.empty())
		upReturnSounds.push_back(wav);
}


void SoundEngine::playDownSound(int keyType) {
	char *filestr;
	switch (keyType) {
	case VK_RETURN:
		filestr = downReturnSounds[rand() % downReturnSounds.size()];
		break;
	case VK_SPACE:
		filestr = downSpaceSounds[rand() % downSpaceSounds.size()];
		break;
	default:
		filestr = downSounds[rand() % downSounds.size()];
		break;
	}
	PlaySound(filestr, NULL, SND_ASYNC | SND_MEMORY);
}

void SoundEngine::playUpSound(int keyType) {
	char *filestr;
	switch (keyType) {
	case VK_RETURN:
		filestr = upReturnSounds[rand() % upReturnSounds.size()];
		break;
	case VK_SPACE:
		filestr = upSpaceSounds[rand() % upSpaceSounds.size()];
		break;
	default:
		filestr = upSounds[rand() % upSounds.size()];
		break;
	}
	PlaySound(filestr, NULL, SND_ASYNC | SND_MEMORY);
}

char *readWavFile(char *str) {
	std::ifstream fwav(str, std::ifstream::binary);
	if (!fwav)
		return NULL;

	fwav.seekg(0, std::ifstream::end);   // get length of file
	int length = fwav.tellg();
	char *wav = new char[length];    // allocate memory
	fwav.seekg(0, std::ifstream::beg);   // position to start of file
	fwav.read(wav, length);  // read entire file

	fwav.close();
	return wav;
}