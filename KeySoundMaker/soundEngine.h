#ifndef SOUND_ENGINE
#define SOUND_ENGINE
#include <vector>
class SoundEngine {
private:
	std::vector<char*> downSounds, downSpaceSounds, downReturnSounds, upSounds, upSpaceSounds, upReturnSounds;
	char *_dirStr;

public:
	SoundEngine(char*);
	void playDownSound(int);
	void playUpSound(int);
};

char *readWavFile(char*);
#endif