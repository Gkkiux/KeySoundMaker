#ifndef SOUND_ENGINE
#define SOUND_ENGINE
# include <dirent.h>
# include <vector>
class SoundEngine {
private:
	std::vector<char*> downSounds, downSpaceSounds, downReturnSounds, upSounds, upSpaceSounds, upReturnSounds;
	//char *_dirStr;

public:
	SoundEngine(char*);
	~SoundEngine();
	void playDownSound(int);
	void playUpSound(int);
	int soundsLoaded();
};

char *readWavFile(char*);
void deleteSounds(std::vector<char*>);
#endif