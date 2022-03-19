/*
 * MIDI CCRecorder plugin based on DISTRHO Plugin Framework (DPF)
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright (C) 2019 Christopher Arndt <info@chrisarndt.de>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef PLUGIN_MIDICCRECORDER_H
#define PLUGIN_MIDICCRECORDER_H

#include "DistrhoPlugin.hpp"

START_NAMESPACE_DISTRHO

#ifndef MAP
#define MAP(v, imin, imax, omin, omax) (((v) - (imin)) * ((omax) - (omin)) / ((imax) - (imin)) + (omin))
#endif

#ifndef MIN
#define MIN(a,b) ( (a) < (b) ? (a) : (b) )
#endif

#ifndef MAX
#define MAX(a,b) ( (a) > (b) ? (a) : (b) )
#endif

#ifndef CLAMP
#define CLAMP(v, min, max) (MIN((max), MAX((min), (v))))
#endif

#define MIDI_CONTROL_CHANGE 0xB0
#define MIDI_PROGRAM_CHANGE 0xC0
#define NUM_CHANNELS 16
#define NUM_CONTROLLERS 128

// -----------------------------------------------------------------------

class PluginMIDICCRecorder : public Plugin {
public:
    enum Parameters {
        paramRecordEnable = 0,
        paramTrigClear,
        paramTrigSend,
        paramTrigTransport,
        paramTrigPCChannel,
        paramTrigPC,
        paramSendChannel,
        paramSendInterval,
        paramCount
    };

    PluginMIDICCRecorder();

protected:
    // -------------------------------------------------------------------
    // Information

    const char* getLabel() const noexcept override {
        return "MIDICCRecorder";
    }

    const char* getDescription() const override {
        return "Store received Control Change messages and replay them when triggered";
    }

    const char* getMaker() const noexcept override {
        return "chrisarndt.de";
    }

    const char* getHomePage() const override {
        return "https://chrisarndt.de/plugins/midiccrecorder";
    }

    const char* getLicense() const noexcept override {
        return "https://spdx.org/licenses/MIT";
    }

    uint32_t getVersion() const noexcept override {
        return d_version(0, 3, 1);
    }

    // Go to:
    //
    // http://service.steinberg.de/databases/plugin.nsf/plugIn
    //
    // Get a proper plugin UID and fill it in here!
    int64_t getUniqueId() const noexcept override {
        return d_cconst('M', 'C', 'c', 'r');
    }

    // -------------------------------------------------------------------
    // Init

    void initParameter(uint32_t index, Parameter& parameter) override;
    void initProgramName(uint32_t index, String& programName) override;
    void initState(uint32_t index, String& stateKey, String& defaultStateValue) override;

    // -------------------------------------------------------------------
    // Internal data

    float getParameterValue(uint32_t index) const override;
    void setParameterValue(uint32_t index, float value) override;
    void loadProgram(uint32_t index) override;
    String getState(const char* key) const override;
    void setState(const char* key, const char* value) override;
    void clearState();

    // -------------------------------------------------------------------
    // Optional

    // Optional callback to inform the plugin about a sample rate change.
    void sampleRateChanged(double newSampleRate) override;

    // -------------------------------------------------------------------
    // Process

    void activate() override;
    void startSend();
    void run(const float**, float**, uint32_t,
             const MidiEvent* midiEvents, uint32_t midiEventCount) override;

    // -------------------------------------------------------------------

private:
    float fParams[paramCount];
    double fSampleRate;
    uint8_t stateCC[NUM_CHANNELS][NUM_CONTROLLERS];
    uint8_t curChan, curCC, sendChannel;
    bool playing, sendInProgress;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginMIDICCRecorder)
};

struct Preset {
    const char* name;
    float params[PluginMIDICCRecorder::paramCount];
};

Preset factoryPresets[] = {
    {
        "Default",
        {0, 0, 0, 0, 17, 0, 0, 1.0}
    },
};

constexpr uint presetCount = sizeof(factoryPresets) / sizeof(Preset);
constexpr uint stateCount = NUM_CHANNELS;

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO

#endif  // #ifndef PLUGIN_MIDICCRECORDER_H
