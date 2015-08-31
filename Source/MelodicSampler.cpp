#include "MelodicSampler.h"

static MP3AudioFormat *mp3Format = new MP3AudioFormat();

MelodicSamplerSound::MelodicSamplerSound (String& filePath,
                            const int beginSamples,
                            const int endSamples) {
    File *file = new File(filePath);
    FileInputSource *source = new FileInputSource(*file, false);
    InputStream *stream = source->createInputStream();
    AudioFormatReader *reader = mp3Format->createReaderFor(stream, true);
    sourceSampleRate = reader->sampleRate;

    attackSamples = 0, releaseSamples = 0;
    // attackSamples = roundToInt (attackTimeSec * sourceSampleRate);
    // releaseSamples = roundToInt (releaseTimeSec * sourceSampleRate);
    if (sourceSampleRate <= 0 || reader->lengthInSamples <= 0) {
        length = 0;
    } else {
        length = jmin ((int) reader->lengthInSamples, endSamples);
        data = new AudioSampleBuffer (jmin (2, (int) reader->numChannels), length + 4);
        reader->read (data, 0, length + 4, 0, true, true);
    }
}

MelodicSamplerSound::~MelodicSamplerSound() {
}

bool MelodicSamplerSound::appliesToNote (int /*midiNoteNumber*/) {
    return true; // applies to all notes
}

bool MelodicSamplerSound::appliesToChannel (int /*midiChannel*/) {
    return true; // applies to all channels
}

//==============================================================================
MelodicSamplerVoice::MelodicSamplerVoice() :
pitchRatio (0.0),
sourceSamplePosition (0.0),
lgain (0.0f), rgain (0.0f),
attackReleaseLevel (0), attackDelta (0), releaseDelta (0),
isInAttack (false), isInRelease (false) {
}

MelodicSamplerVoice::~MelodicSamplerVoice() {
}

bool MelodicSamplerVoice::canPlaySound (SynthesiserSound* sound) {
    return dynamic_cast<const MelodicSamplerSound*> (sound) != nullptr;
}

void MelodicSamplerVoice::startNote (const int midiNoteNumber,
                              const float velocity,
                              SynthesiserSound* s,
                              const int currentPitchWheelPosition) {
    const MelodicSamplerSound* const sound = dynamic_cast <const MelodicSamplerSound*> (s);
    pitchRatio = pow (2.0, midiNoteNumber / 12.0)
    *sound->sourceSampleRate / getSampleRate();
    
    sourceSamplePosition = 0.0;
    lgain = velocity;
    rgain = velocity;
    
    isInAttack = (sound->attackSamples > 0);
    isInRelease = false;
    
    if (isInAttack) {
        attackReleaseLevel = 0.0f;
        attackDelta = (float) (pitchRatio / sound->attackSamples);
    }
    else {
        attackReleaseLevel = 1.0f;
        attackDelta = 0.0f;
    }
    
    if (sound->releaseSamples > 0)
        releaseDelta = (float) (-pitchRatio / sound->releaseSamples);
    else
        releaseDelta = -1.0f;
}

void MelodicSamplerVoice::stopNote (float /*velocity*/, bool allowTailOff) {
    if (allowTailOff) {
        isInAttack = false;
        isInRelease = true;
    }
    else {
        clearCurrentNote();
    }
}

void MelodicSamplerVoice::pitchWheelMoved (const int /*newValue*/) {
}

void MelodicSamplerVoice::controllerMoved (const int /*controllerNumber*/,
                                    const int /*newValue*/) {
}

//==============================================================================
void MelodicSamplerVoice::renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples) {
    const MelodicSamplerSound* const playingSound =
                    static_cast <MelodicSamplerSound*>(getCurrentlyPlayingSound().get());
    if (playingSound == nullptr)
        return;
    const float* const inL = playingSound->data->getReadPointer (0);
    const float* const inR = playingSound->data->getNumChannels() > 1
    ? playingSound->data->getReadPointer (1) : nullptr;
    
    float* outL = outputBuffer.getWritePointer (0, startSample);
    float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer (1, startSample) : nullptr;
    
    while (--numSamples >= 0) {
        const int pos = (int) sourceSamplePosition;
        const float alpha = (float) (sourceSamplePosition - pos);
        const float invAlpha = 1.0f - alpha;
        
        // just using a very simple linear interpolation here..
        float l = (inL [pos] * invAlpha + inL [pos + 1] * alpha);
        float r = (inR != nullptr) ? (inR [pos] * invAlpha + inR [pos + 1] * alpha)
        : l;
        
        l *= lgain;
        r *= rgain;
        
        if (isInAttack) {
            l *= attackReleaseLevel;
            r *= attackReleaseLevel;
            
            attackReleaseLevel += attackDelta;
            
            if (attackReleaseLevel >= 1.0f) {
                attackReleaseLevel = 1.0f;
                isInAttack = false;
            }
        } else if (isInRelease) {
            l *= attackReleaseLevel;
            r *= attackReleaseLevel;
            
            attackReleaseLevel += releaseDelta;
            
            if (attackReleaseLevel <= 0.0f) {
                stopNote (0.0f, false);
                break;
            }
        }
        
        if (outR != nullptr) {
            *outL++ += l;
            *outR++ += r;
        } else {
            *outL++ += (l + r) * 0.5f;
        }
        
        sourceSamplePosition += pitchRatio;
        
        if (sourceSamplePosition > playingSound->length) {
            stopNote (0.0f, false);
            break;
        }
    }
}
