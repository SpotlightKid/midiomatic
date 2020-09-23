/*
 * MIDI CC To Pressure plugin based on DISTRHO Plugin Framework (DPF)
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright (C) 2020 Christopher Arndt <info@chrisarndt.de>
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

#ifndef PLUGIN_MIDICCTOPRESSURE_H
#define PLUGIN_MIDICCTOPRESSURE_H

#include "DistrhoPlugin.hpp"

START_NAMESPACE_DISTRHO

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
#define MIDI_CHANNEL_PRESSURE 0xD0

// -----------------------------------------------------------------------

class PluginMIDICCToPressure : public Plugin {
public:
    enum Parameters {
        paramFilterChannel,
        paramKeepOriginal,
        paramSrcCC,
        paramCount
    };

    PluginMIDICCToPressure();

protected:
    // -------------------------------------------------------------------
    // Information

    const char* getLabel() const noexcept override {
        return "MIDICCToPressure";
    }

    const char* getDescription() const override {
        return "Convert Control Change messages into monophonic Channel Pressure (Aftertouch)";
    }

    const char* getMaker() const noexcept override {
        return "chrisarndt.de";
    }

    const char* getHomePage() const override {
        return "https://chrisarndt.de/plugins/midicctopressure";
    }

    const char* getLicense() const noexcept override {
        return "https://spdx.org/licenses/MIT";
    }

    uint32_t getVersion() const noexcept override {
        return d_version(1, 0, 0);
    }

    // Go to:
    //
    // http://service.steinberg.de/databases/plugin.nsf/plugIn
    //
    // Get a proper plugin UID and fill it in here!
    int64_t getUniqueId() const noexcept override {
        return d_cconst('M', 'C', 't', 'A');
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

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginMIDICCToPressure)
};

struct Preset {
    const char* name;
    const float params[PluginMIDICCToPressure::paramCount];
};

const Preset factoryPresets[] = {
    {
        "Modulation",
        {0.0, 0.0, 1.0}
    },
    {
        "Breath",
        {0.0, 0.0, 2.0}
    },
    {
        "Foot Controller",
        {0.0, 0.0, 4.0}
    },
    {
        "Expression",
        {0.0, 0.0, 11.0}
    }
};

const uint presetCount = sizeof(factoryPresets) / sizeof(Preset);

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO

#endif  // #ifndef PLUGIN_MIDICCTOPRESSURE_H
