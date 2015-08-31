#include "NoteToSampleTranslator.h"

// array of MelodicSamplerSound-queues (one queue for each MIDI note)
Array<MelodicSamplerSound*> sampleQueueForNote[NUM_MIDI_NOTES];

NoteToSampleTranslator::NoteToSampleTranslator(String& noteToSampleMapPath) {
    File *noteToSampleMapFile = new File(noteToSampleMapPath);
    FileInputStream *inputStream = noteToSampleMapFile->createInputStream();
    for (int note = 0; note < NUM_MIDI_NOTES; note++) {
        sampleQueueForNote[note] = *new Array<MelodicSamplerSound*>;
        sampleQueueForNote[note].add(new MelodicSamplerSound(*new String("/Users/khiner/Development/aubio/The Beatles-Hey Jude.mp3"), note, 50000, 50000));
    }
    //populateSampleQueues(inputStream);
}

NoteToSampleTranslator::~NoteToSampleTranslator() {
    for (int i = 0; i < NUM_MIDI_NOTES; i++) {
        sampleQueueForNote[i].clear();
    }
}

MelodicSamplerSound* NoteToSampleTranslator::sampleForNote(int note) {
    return sampleQueueForNote[note].getFirst();
}

void NoteToSampleTranslator::populateSampleQueues(FileInputStream *inputStream) {
    while (inputStream->readByte() != '_'); // get past metadata
    inputStream->readByte(); // newline before the good stuff
    while (!inputStream->isExhausted()) {
        int note = inputStream->readInt();
        inputStream->readByte(); // =
        inputStream->readByte(); // >
        String line = inputStream->readNextLine();
        
        StringArray sampleInfos;
        sampleInfos.addTokens (line, "||", "\"");
        for (int i = 0; i < sampleInfos.size(); i++) {
            String sampleInfo = sampleInfos[i];
            StringArray sampleInfoComponents;
            sampleInfoComponents.addTokens(sampleInfo, "::", "\"");
            if (sampleInfoComponents.size() != 3) {
                // something went wrong. skip
                continue;
            }
            String path = sampleInfoComponents[0];
            int beginSamples = sampleInfoComponents[1].getIntValue();
            int durationSamples = sampleInfoComponents[2].getIntValue();
            MelodicSamplerSound *sound = new MelodicSamplerSound(path, note, beginSamples, durationSamples);
            sampleQueueForNote[note].add(sound);
        }
    }
}
