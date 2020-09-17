/*
 * MIDI CC Map X4 plugin based on DISTRHO Plugin Framework (DPF)
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

#ifndef PLUGIN_MIDICCMAPX4_H
#define PLUGIN_MIDICCMAPX4_H

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

// -----------------------------------------------------------------------

class PluginMIDICCMapX4 : public Plugin {
public:
    enum Parameters {
        paramFilterChannel,
        paramCCSource,
        paramKeepOriginal,
        paramCC1Mode,
        paramCC1Dest,
        paramCC1Channel,
        paramCC1FilterDups,
        paramCC1Start,
        paramCC1End,
        paramCC1Min,
        paramCC1Max,
        paramCC2Mode,
        paramCC2Dest,
        paramCC2Channel,
        paramCC2FilterDups,
        paramCC2Start,
        paramCC2End,
        paramCC2Min,
        paramCC2Max,
        paramCC3Mode,
        paramCC3Dest,
        paramCC3Channel,
        paramCC3FilterDups,
        paramCC3Start,
        paramCC3End,
        paramCC3Min,
        paramCC3Max,
        paramCC4Mode,
        paramCC4Dest,
        paramCC4Channel,
        paramCC4FilterDups,
        paramCC4Start,
        paramCC4End,
        paramCC4Min,
        paramCC4Max,
        paramCount
    };

    PluginMIDICCMapX4();

protected:
    // -------------------------------------------------------------------
    // Information

    const char* getLabel() const noexcept override {
        return "MIDICCMapX4";
    }

    const char* getDescription() const override {
        return "Map a single input CC to up to four output CCs";
    }

    const char* getMaker() const noexcept override {
        return "chrisarndt.de";
    }

    const char* getHomePage() const override {
        return DISTRHO_PLUGIN_URI;
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
        return d_cconst('M', 'C', 'C', '4');
    }

    // -------------------------------------------------------------------
    // Init

    void initParameter(uint32_t index, Parameter& parameter) override;
    void initPortGroup(uint32_t index, PortGroup& pgroup) override;
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
    int8_t lastCCValue[16][128];

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginMIDICCMapX4)
};

struct Preset {
    const char* name;
    const float params[PluginMIDICCMapX4::paramCount];
};

const Preset factoryPresets[] = {
    {
        "Default",
        {
            0.0,    // Filter Channel
            0.0,    // Keep Source CC
            1.0,    // Source CC
            0.0,    // CC1 Mode
            14.0,   // CC1 Dest
            0.0,    // CC1 Channel
            1.0,    // CC1 Keep Dups
            0.0,    // CC1 Start
            127.0,  // CC1 End
            0.0,    // CC1 Min
            127.0,  // CC1 Max
            0.0,    // CC2 Mode
            15.0,   // CC2 Dest
            0.0,    // CC2 Channel
            1.0,    // CC2 Keep Dups
            0.0,    // CC2 Start
            127.0,  // CC2 End
            0.0,    // CC2 Min
            127.0,  // CC2 Max
            0.0,    // CC3 Mode
            16.0,   // CC3 Dest
            0.0,    // CC3 Channel
            1.0,    // CC3 Keep Dups
            0.0,    // CC3 Start
            127.0,  // CC3 End
            0.0,    // CC3 Min
            127.0,  // CC3 Max
            0.0,    // CC4 Mode
            17.0,   // CC4 Dest
            0.0,    // CC4 Channel
            1.0,    // CC4 Keep Dups
            0.0,    // CC4 Start
            127.0,  // CC4 End
            0.0,    // CC4 Min
            127.0,  // CC4 Max
        }
    },
};

const uint presetCount = sizeof(factoryPresets) / sizeof(Preset);

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO

#endif  // #ifndef PLUGIN_MIDICCMAPX4_H
