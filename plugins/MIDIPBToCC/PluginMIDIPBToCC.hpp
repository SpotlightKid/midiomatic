/*
 * MIDI PBToCC plugin based on DISTRHO Plugin Framework (DPF)
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

#ifndef PLUGIN_MIDIPBTOCC_H
#define PLUGIN_MIDIPBTOCC_H

#include "DistrhoPlugin.hpp"

START_NAMESPACE_DISTRHO

#ifndef IN_RANGE
#define IN_RANGE(v, min, max) ((min) <= (max) ? ((v) >= (min) && (v) <= (max)) : ((v) >= (min) || (v) <= (max)))
#endif

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
#define MIDI_PITCH_BEND 0xE0

// -----------------------------------------------------------------------

class PluginMIDIPBToCC : public Plugin {
public:
    enum Parameters {
        paramFilterChannel,
        paramKeepOriginal,
        paramPBMin,
        paramPBMax,
        paramCC1,
        paramCC1Min,
        paramCC1Max,
        paramCC2,
        paramCC2Min,
        paramCC2Max,
        paramCount
    };

    PluginMIDIPBToCC();

protected:
    // -------------------------------------------------------------------
    // Information

    const char* getLabel() const noexcept override {
        return "MIDIPBToCC";
    }

    const char* getDescription() const override {
        return "Convert Pitch Bend into Control Change messages";
    }

    const char* getMaker() const noexcept override {
        return "chrisarndt.de";
    }

    const char* getHomePage() const override {
        return "https://chrisarndt.de/plugins/midipbtocc";
    }

    const char* getLicense() const noexcept override {
        return "https://spdx.org/licenses/MIT";
    }

    uint32_t getVersion() const noexcept override {
        return d_version(0, 1, 0);
    }

    // Go to:
    //
    // http://service.steinberg.de/databases/plugin.nsf/plugIn
    //
    // Get a proper plugin UID and fill it in here!
    int64_t getUniqueId() const noexcept override {
        return d_cconst('M', 'P', 'b', 'C');
    }

    // -------------------------------------------------------------------
    // Init

    void initParameter(uint32_t index, Parameter& parameter) override;
    void initProgramName(uint32_t index, String& programName) override;

    // -------------------------------------------------------------------
    // Internal data

    float getParameterValue(uint32_t index) const override;
    void setParameterValue(uint32_t index, float value) override;
    void loadProgram(uint32_t index) override;

    // -------------------------------------------------------------------
    // Optional

    // Optional callback to inform the plugin about a sample rate change.
    void sampleRateChanged(double newSampleRate) override;

    // -------------------------------------------------------------------
    // Process

    void activate() override;

    void run(const float**, float**, uint32_t,
             const MidiEvent* midiEvents, uint32_t midiEventCount) override;


    // -------------------------------------------------------------------

private:
    float fParams[paramCount];
    int8_t filterChannel;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginMIDIPBToCC)
};

struct Preset {
    const char* name;
    const float params[PluginMIDIPBToCC::paramCount];
};

const Preset factoryPresets[] = {
    {
        "Modulation +/-",
        {0.0, 0.0, -8192.0, 8191.0, 1.0, 0.0, 127.0, 1.0, 0.0, 127.0}
    },
    {
        "PB + / Mod -",
        {0.0, 0.0, -8192.0, -1, 1.0, 0.0, 127.0, 1.0, 0.0, 127.0}
    }
};

const uint presetCount = sizeof(factoryPresets) / sizeof(Preset);

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO

#endif  // #ifndef PLUGIN_MIDIPBTOCC_H
