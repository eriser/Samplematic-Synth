#ifndef MELODIC_SAMPLER_H_INCLUDED
#define MELODIC_SAMPLER_H_INCLUDED

#include "JuceHeader.h"
//==============================================================================
/**
 A subclass of SynthesiserSound that represents a sampled audio clip.
 
 This is a pretty basic sampler, and just attempts to load the whole audio stream
 into memory.
 
 To use it, create a Synthesiser, add some SamplerVoice objects to it, then
 give it some SampledSound objects to play.
 
 @see SamplerVoice, Synthesiser, SynthesiserSound
 */
class MelodicSamplerSound : public SynthesiserSound {
public:
    //==============================================================================
    /** Creates a sampled sound from an audio reader.
     
     This will attempt to load the audio from the source into memory and store
     it in this object.
     
     @param filePath       the absolute path of the audio to load.
     @param beginSamples   the sample to begin playback
     @param beginSamples   the sample to end/loop playback
     */
    MelodicSamplerSound (String& filePath,
                  int beginSamples,
                  int endSamples);
    
    /** Destructor. */
    ~MelodicSamplerSound();
    
    /** Returns the audio sample data.
     This could return nullptr if there was a problem loading the data.
     */
    AudioSampleBuffer* getAudioData() const noexcept        { return data; }
    
    
    //==============================================================================
    bool appliesToNote (int midiNoteNumber) override;
    bool appliesToChannel (int midiChannel) override;
    
    
private:
    //==============================================================================
    friend class MelodicSamplerVoice;
    
    ScopedPointer<AudioSampleBuffer> data;
    double sourceSampleRate;
    int length, attackSamples, releaseSamples;
};


//==============================================================================
/**
 A subclass of SynthesiserVoice that can play a SamplerSound.
 
 To use it, create a Synthesiser, add some SamplerVoice objects to it, then
 give it some SampledSound objects to play.
 
 @see SamplerSound, Synthesiser, SynthesiserVoice
 */
class MelodicSamplerVoice : public SynthesiserVoice {
public:
    //==============================================================================
    /** Creates a SamplerVoice. */
    MelodicSamplerVoice();
    
    /** Destructor. */
    ~MelodicSamplerVoice();
    
    //==============================================================================
    bool canPlaySound (SynthesiserSound*) override;
    
    void startNote (int midiNoteNumber, float velocity, SynthesiserSound*, int pitchWheel) override;
    void stopNote (float velocity, bool allowTailOff) override;
    
    void pitchWheelMoved (int newValue) override;
    void controllerMoved (int controllerNumber, int newValue) override;
    
    void renderNextBlock (AudioSampleBuffer&, int startSample, int numSamples) override;
    
    
private:
    //==============================================================================
    double pitchRatio;
    double sourceSamplePosition;
    float lgain, rgain, attackReleaseLevel, attackDelta, releaseDelta;
    bool isInAttack, isInRelease;
};


#endif   // MELODIC_SAMPLER_H_INCLUDED
