#ifndef NOTE_TO_SAMPLE_TRANSLATOR_H_INCLUDED
#define NOTE_TO_SAMPLE_TRANSLATOR_H_INCLUDED

#include "JuceHeader.h"
#include "MelodicSampler.h"

class NoteToSampleTranslator {
public:
    NoteToSampleTranslator(String& noteToSampleMapPath);
    ~NoteToSampleTranslator();

    MelodicSamplerSound *sampleForNote(int note);

private:
    void populateSampleQueues(FileInputStream *inputStream);
};

#endif // NOTE_TO_SAMPLE_TRANSLATOR_H_INCLUDED
