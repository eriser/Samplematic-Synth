#include "PluginProcessor.h"
#include "PluginEditor.h"

Synthesiser *synthesiser;
NoteToSampleTranslator *noteToSampleTranslator;
SamplematicSynthAudioProcessor::SamplematicSynthAudioProcessor() {
    // TODO get from user input (file browser selection)
    String *noteToSampleDataMapPath = new String("/Users/khiner/Development/aubio/rubynote_to_sample_info_map.txt");
    noteToSampleTranslator = new NoteToSampleTranslator(*noteToSampleDataMapPath);
    synthesiser = new Synthesiser();
    MelodicSamplerVoice *voice = new MelodicSamplerVoice();
    synthesiser->addVoice(voice);
    for (int note = 0; note < NUM_MIDI_NOTES; note++) {
        MelodicSamplerSound *sound = noteToSampleTranslator->sampleForNote(note);
        synthesiser->addSound(sound);
    }
}

SamplematicSynthAudioProcessor::~SamplematicSynthAudioProcessor() {
//    delete(noteToSampleTranslator);
    delete(synthesiser);
}

const String SamplematicSynthAudioProcessor::getName() const {
    return JucePlugin_Name;
}

int SamplematicSynthAudioProcessor::getNumParameters() {
    return 0;
}

float SamplematicSynthAudioProcessor::getParameter (int index) {
    return 0.0f;
}

void SamplematicSynthAudioProcessor::setParameter (int index, float newValue) {
}

const String SamplematicSynthAudioProcessor::getParameterName (int index) {
    return String();
}

const String SamplematicSynthAudioProcessor::getParameterText (int index) {
    return String();
}

const String SamplematicSynthAudioProcessor::getInputChannelName (int channelIndex) const {
    return String (channelIndex + 1);
}

const String SamplematicSynthAudioProcessor::getOutputChannelName (int channelIndex) const {
    return String (channelIndex + 1);
}

bool SamplematicSynthAudioProcessor::isInputChannelStereoPair (int index) const {
    return true;
}

bool SamplematicSynthAudioProcessor::isOutputChannelStereoPair (int index) const {
    return true;
}

bool SamplematicSynthAudioProcessor::acceptsMidi() const {
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SamplematicSynthAudioProcessor::producesMidi() const {
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SamplematicSynthAudioProcessor::silenceInProducesSilenceOut() const {
    return false;
}

double SamplematicSynthAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int SamplematicSynthAudioProcessor::getNumPrograms() {
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SamplematicSynthAudioProcessor::getCurrentProgram() {
    return 0;
}

void SamplematicSynthAudioProcessor::setCurrentProgram (int index) {
}

const String SamplematicSynthAudioProcessor::getProgramName (int index) {
    return String();
}

void SamplematicSynthAudioProcessor::changeProgramName (int index, const String& newName) {
}

//==============================================================================
void SamplematicSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void SamplematicSynthAudioProcessor::releaseResources() {
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void SamplematicSynthAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages) {
    for (int channel = 0; channel < getNumInputChannels(); ++channel) {
        for (int samp = 0; samp < buffer.getNumSamples(); samp++) {
        }
    }
    synthesiser->renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool SamplematicSynthAudioProcessor::hasEditor() const {
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SamplematicSynthAudioProcessor::createEditor() {
    return new SamplematicSynthAudioProcessorEditor (this);
}

//==============================================================================
void SamplematicSynthAudioProcessor::getStateInformation (MemoryBlock& destData) {
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SamplematicSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new SamplematicSynthAudioProcessor();
}
