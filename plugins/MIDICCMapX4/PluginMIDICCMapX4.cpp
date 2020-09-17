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

#include "PluginMIDICCMapX4.hpp"
#include <algorithm>

START_NAMESPACE_DISTRHO

enum {
    kPortGroupSource,
    kPortGroupCC1,
    kPortGroupCC2,
    kPortGroupCC3,
    kPortGroupCC4,
};

const ParameterEnumerationValue paramEnumSrcChannels[] = {
    {0,"Any"},
    {1, "Channel 1"},
    {2, "Channel 2"},
    {3, "Channel 3"},
    {4, "Channel 4"},
    {5, "Channel 5"},
    {6, "Channel 6"},
    {7, "Channel 7"},
    {8, "Channel 8"},
    {9, "Channel 9"},
    {10, "Channel 10"},
    {11, "Channel 11"},
    {12, "Channel 12"},
    {13, "Channel 13"},
    {14, "Channel 14"},
    {15, "Channel 15"},
    {16, "Channel 16"}
};

const ParameterEnumerationValue paramEnumDstChannels[] = {
    {0,"Same as source"},
    {1, "Channel 1"},
    {2, "Channel 2"},
    {3, "Channel 3"},
    {4, "Channel 4"},
    {5, "Channel 5"},
    {6, "Channel 6"},
    {7, "Channel 7"},
    {8, "Channel 8"},
    {9, "Channel 9"},
    {10, "Channel 10"},
    {11, "Channel 11"},
    {12, "Channel 12"},
    {13, "Channel 13"},
    {14, "Channel 14"},
    {15, "Channel 15"},
    {16, "Channel 16"}
};

const ParameterEnumerationValue paramEnumModes[] {
    {0, "Disabled"},
    {1, "Map start/end range to min/max"},
    {2, "Map full value range to min/max"}
};

// -----------------------------------------------------------------------

template <size_t N>
static inline void fillEnumValues(ParameterEnumerationValues& pev,
                                  const ParameterEnumerationValue(& list)[N]) {
    ParameterEnumerationValue* values = new ParameterEnumerationValue[N];
    pev.count = N;
    pev.values = values;
    std::copy(list, list + N, values);
}

// -----------------------------------------------------------------------

PluginMIDICCMapX4::PluginMIDICCMapX4()
    : Plugin(paramCount, presetCount, 0)  // 0 states
{
    for (uint8_t ch=0; ch<16; ch++) {
        for (uint8_t cc=0; cc<128; cc++) {
            lastCCValue[ch][cc] = -1;
        }
    }
    loadProgram(0);
}

// -----------------------------------------------------------------------
// Init

void PluginMIDICCMapX4::initParameter(uint32_t index, Parameter& parameter) {
    if (index >= paramCount)
        return;

    parameter.hints = kParameterIsAutomable | kParameterIsInteger;
    parameter.ranges.def = 0;
    parameter.ranges.min = 0;
    parameter.ranges.max = 127;

    switch (index) {
        case paramFilterChannel:
            parameter.name = "Filter Channel";
            parameter.symbol = "channelf";
            parameter.ranges.max = 16;
            parameter.enumValues.restrictedMode = true;
            fillEnumValues(parameter.enumValues, paramEnumSrcChannels);
            parameter.group = kPortGroupSource;
            break;
        case paramCCSource:
            parameter.name = "Source CC";
            parameter.symbol = "cc_source";
            parameter.ranges.def = 1;
            parameter.group = kPortGroupSource;
            break;
        case paramKeepOriginal:
            parameter.name = "Keep Source CC Events";
            parameter.shortName = "Keep src. CC";
            parameter.symbol = "keep_original";
            parameter.hints |= kParameterIsBoolean;
            parameter.ranges.max = 1;
            parameter.group = kPortGroupSource;
            break;
        case paramCC1Mode:
            parameter.name = "CC 1 Mode";
            parameter.shortName = "CC1 Mode";
            parameter.symbol = "cc1_mode";
            parameter.ranges.max = 2;
            parameter.enumValues.restrictedMode = true;
            fillEnumValues(parameter.enumValues, paramEnumModes);
            parameter.group = kPortGroupCC1;
            break;
        case paramCC1Dest:
            parameter.name = "CC 1 Destination";
            parameter.shortName = "CC1 Dest.";
            parameter.symbol = "cc1_dest";
            parameter.ranges.def = 14;
            parameter.group = kPortGroupCC1;
            break;
        case paramCC1Channel:
            parameter.name = "CC1 Channel";
            parameter.symbol = "cc1_chan";
            parameter.ranges.max = 16;
            parameter.enumValues.restrictedMode = true;
            fillEnumValues(parameter.enumValues, paramEnumDstChannels);
            parameter.group = kPortGroupCC1;
            break;
        case paramCC1FilterDups:
            parameter.name = "CC 1 Filter repeated values";
            parameter.shortName = "CC1 Filter dups";
            parameter.symbol = "cc1_filterdups";
            parameter.ranges.def = 1;
            parameter.ranges.max = 1;
            parameter.hints |= kParameterIsBoolean;
            parameter.group = kPortGroupCC1;
            break;
        case paramCC1Start:
            parameter.name = "CC 1 Start";
            parameter.shortName = "CC1 Start";
            parameter.symbol = "cc1_start";
            parameter.group = kPortGroupCC1;
            break;
        case paramCC1End:
            parameter.name = "CC 1 End";
            parameter.shortName = "CC1 End";
            parameter.symbol = "cc1_end";
            parameter.ranges.def = 127;
            parameter.group = kPortGroupCC1;
            break;
        case paramCC1Min:
            parameter.name = "CC 1 Minimum value";
            parameter.shortName = "CC1 Min. value";
            parameter.symbol = "cc1_min";
            parameter.group = kPortGroupCC1;
            break;
        case paramCC1Max:
            parameter.name = "CC 1 Maximum value";
            parameter.shortName = "CC1 Max. value";
            parameter.symbol = "cc1_max";
            parameter.ranges.def = 127;
            parameter.group = kPortGroupCC1;
            break;
        case paramCC2Mode:
            parameter.name = "CC 2 Mode";
            parameter.shortName = "CC2 Mode";
            parameter.symbol = "cc2_mode";
            parameter.ranges.max = 2;
            parameter.enumValues.restrictedMode = true;
            fillEnumValues(parameter.enumValues, paramEnumModes);
            parameter.group = kPortGroupCC2;
            break;
        case paramCC2Dest:
            parameter.name = "CC 2 Destination";
            parameter.shortName = "CC2 Dest.";
            parameter.symbol = "cc2_dest";
            parameter.ranges.def = 15;
            parameter.group = kPortGroupCC2;
            break;
        case paramCC2Channel:
            parameter.name = "CC2 Channel";
            parameter.symbol = "cc2_chan";
            parameter.ranges.max = 16;
            parameter.enumValues.restrictedMode = true;
            fillEnumValues(parameter.enumValues, paramEnumDstChannels);
            parameter.group = kPortGroupCC2;
            break;
        case paramCC2FilterDups:
            parameter.name = "CC 2 Filter repeated values";
            parameter.shortName = "CC2 Filter dups";
            parameter.symbol = "cc2_filterdups";
            parameter.ranges.def = 1;
            parameter.ranges.max = 1;
            parameter.hints |= kParameterIsBoolean;
            parameter.ranges.max = 1;
            parameter.group = kPortGroupCC2;
            break;
        case paramCC2Start:
            parameter.name = "CC 2 Start";
            parameter.shortName = "CC2 Start";
            parameter.symbol = "cc2_start";
            parameter.group = kPortGroupCC2;
            break;
        case paramCC2End:
            parameter.name = "CC 2 End";
            parameter.shortName = "CC2 End";
            parameter.symbol = "cc2_end";
            parameter.ranges.def = 127;
            parameter.group = kPortGroupCC2;
            break;
        case paramCC2Min:
            parameter.name = "CC 2 Minimum value";
            parameter.shortName = "CC2 Min. value";
            parameter.symbol = "cc2_min";
            parameter.group = kPortGroupCC2;
            break;
        case paramCC2Max:
            parameter.name = "CC 2 Maximum value";
            parameter.shortName = "CC2 Max. value";
            parameter.symbol = "cc2_max";
            parameter.ranges.def = 127;
            parameter.group = kPortGroupCC2;
            break;
        case paramCC3Mode:
            parameter.name = "CC 3 Mode";
            parameter.shortName = "CC3 Mode";
            parameter.symbol = "cc3_mode";
            parameter.ranges.max = 2;
            parameter.enumValues.restrictedMode = true;
            fillEnumValues(parameter.enumValues, paramEnumModes);
            parameter.group = kPortGroupCC3;
            break;
        case paramCC3Dest:
            parameter.name = "CC 3 Destination";
            parameter.shortName = "CC3 Dest.";
            parameter.symbol = "cc3_dest";
            parameter.ranges.def = 16;
            parameter.group = kPortGroupCC3;
            break;
        case paramCC3Channel:
            parameter.name = "CC3 Channel";
            parameter.symbol = "cc3_chan";
            parameter.ranges.max = 16;
            parameter.enumValues.restrictedMode = true;
            fillEnumValues(parameter.enumValues, paramEnumDstChannels);
            parameter.group = kPortGroupCC3;
            break;
        case paramCC3FilterDups:
            parameter.name = "CC 3 Filter repeated values";
            parameter.shortName = "CC3 Filter dups";
            parameter.symbol = "cc3_filterdups";
            parameter.ranges.def = 1;
            parameter.ranges.max = 1;
            parameter.hints |= kParameterIsBoolean;
            parameter.group = kPortGroupCC3;
            break;
        case paramCC3Start:
            parameter.name = "CC 3 Start";
            parameter.shortName = "CC3 Start";
            parameter.symbol = "cc3_start";
            parameter.group = kPortGroupCC3;
            break;
        case paramCC3End:
            parameter.name = "CC 3 End";
            parameter.shortName = "CC3 End";
            parameter.symbol = "cc3_end";
            parameter.ranges.def = 127;
            parameter.group = kPortGroupCC3;
            break;
        case paramCC3Min:
            parameter.name = "CC 3 Minimum value";
            parameter.shortName = "CC3 Min. value";
            parameter.symbol = "cc3_min";
            parameter.group = kPortGroupCC3;
            break;
        case paramCC3Max:
            parameter.name = "CC 3 Maximum value";
            parameter.shortName = "CC3 Max. value";
            parameter.symbol = "cc3_max";
            parameter.ranges.def = 127;
            parameter.group = kPortGroupCC3;
            break;
        case paramCC4Mode:
            parameter.name = "CC 4 Mode";
            parameter.shortName = "CC4 Mode";
            parameter.symbol = "cc4_mode";
            parameter.ranges.max = 2;
            parameter.enumValues.restrictedMode = true;
            fillEnumValues(parameter.enumValues, paramEnumModes);
            parameter.group = kPortGroupCC4;
            break;
        case paramCC4Dest:
            parameter.name = "CC 4 Destination";
            parameter.shortName = "CC4 Dest.";
            parameter.symbol = "cc4_dest";
            parameter.ranges.def = 17;
            parameter.group = kPortGroupCC4;
            break;
        case paramCC4Channel:
            parameter.name = "CC4 Channel";
            parameter.symbol = "cc4_chan";
            parameter.ranges.max = 16;
            parameter.enumValues.restrictedMode = true;
            fillEnumValues(parameter.enumValues, paramEnumDstChannels);
            parameter.group = kPortGroupCC4;
            break;
        case paramCC4FilterDups:
            parameter.name = "CC 4 Filter repeated values";
            parameter.shortName = "CC4 Filter dups";
            parameter.symbol = "cc4_filterdups";
            parameter.ranges.def = 1;
            parameter.ranges.max = 1;
            parameter.hints |= kParameterIsBoolean;
            parameter.group = kPortGroupCC4;
            break;
        case paramCC4Start:
            parameter.name = "CC  Start";
            parameter.shortName = "CC4 Start";
            parameter.symbol = "cc4_start";
            parameter.group = kPortGroupCC4;
            break;
        case paramCC4End:
            parameter.name = "CC 4 End";
            parameter.shortName = "CC4 End";
            parameter.symbol = "cc4_end";
            parameter.ranges.def = 127;
            parameter.group = kPortGroupCC4;
            break;
        case paramCC4Min:
            parameter.name = "CC 4 Minimum value";
            parameter.shortName = "CC4 Min. value";
            parameter.symbol = "cc4_min";
            parameter.group = kPortGroupCC4;
            break;
        case paramCC4Max:
            parameter.name = "CC 4 Maximum value";
            parameter.shortName = "CC4 Max. value";
            parameter.symbol = "cc4_max";
            parameter.ranges.def = 127;
            parameter.group = kPortGroupCC4;
            break;
   }
}

/**
  Set the name and symbol of the port group @a index.
  This function will be called once for every port group, shortly after the plugin is created.
*/
void PluginMIDICCMapX4::initPortGroup(uint32_t index, PortGroup& pgroup) {
    switch (index) {
        case kPortGroupSource:
            pgroup.name = "Source";
            pgroup.symbol = "source";
            break;
        case kPortGroupCC1:
            pgroup.name = "Destination #1";
            pgroup.symbol = "dest1";
            break;
        case kPortGroupCC2:
            pgroup.name = "Destination #2";
            pgroup.symbol = "dest2";
            break;
        case kPortGroupCC3:
            pgroup.name = "Destination #3";
            pgroup.symbol = "dest3";
            break;
        case kPortGroupCC4:
            pgroup.name = "Destination #4";
            pgroup.symbol = "dest4";
            break;
    }
}


/**
  Set the name of the program @a index.
  This function will be called once, shortly after the plugin is created.
*/
void PluginMIDICCMapX4::initProgramName(uint32_t index, String& programName) {
    if (index < presetCount) {
        programName = factoryPresets[index].name;
    }
}

// -----------------------------------------------------------------------
// Internal data

/**
  Optional callback to inform the plugin about a sample rate change.
*/
void PluginMIDICCMapX4::sampleRateChanged(double newSampleRate) {
    (void) newSampleRate;
}

/**
  Get the current value of a parameter.
*/
float PluginMIDICCMapX4::getParameterValue(uint32_t index) const {
    return fParams[index];
}

/**
  Change a parameter value.
*/
void PluginMIDICCMapX4::setParameterValue(uint32_t index, float value) {
    switch (index) {
        case paramFilterChannel:
            fParams[index] = CLAMP(value, 0.0f, 16.0f);
            filterChannel = (int8_t) fParams[index] - 1;
            break;
        case paramCC1Channel:
        case paramCC2Channel:
        case paramCC3Channel:
        case paramCC4Channel:
            fParams[index] = CLAMP(value, 0.0f, 16.0f);
            break;
        case paramKeepOriginal:
        case paramCC1FilterDups:
        case paramCC2FilterDups:
        case paramCC3FilterDups:
        case paramCC4FilterDups:
            fParams[index] = CLAMP(value, 0.0f, 1.0f);
            break;
        case paramCC1Mode:
        case paramCC2Mode:
        case paramCC3Mode:
        case paramCC4Mode:
            fParams[index] = CLAMP(value, 0.0f, 2.0f);
            break;
        case paramCCSource:
        case paramCC1Dest:
        case paramCC1Min:
        case paramCC1Max:
        case paramCC1Start:
        case paramCC1End:
        case paramCC2Dest:
        case paramCC2Min:
        case paramCC2Max:
        case paramCC2Start:
        case paramCC2End:
        case paramCC3Dest:
        case paramCC3Min:
        case paramCC3Max:
        case paramCC3Start:
        case paramCC3End:
        case paramCC4Dest:
        case paramCC4Min:
        case paramCC4Max:
        case paramCC4Start:
        case paramCC4End:
            fParams[index] = CLAMP(value, 0.0f, 127.0f);
            break;
    }
}

/**
  Load a program.
  The host may call this function from any context,
  including realtime processing.
*/
void PluginMIDICCMapX4::loadProgram(uint32_t index) {
    if (index < presetCount) {
        for (int i=0; i < paramCount; i++) {
            setParameterValue(i, factoryPresets[index].params[i]);
        }

    }
}

// -----------------------------------------------------------------------
// Process

void PluginMIDICCMapX4::activate() {
    // plugin is activated
}


void PluginMIDICCMapX4::run(const float**, float**, uint32_t,
                            const MidiEvent* events, uint32_t eventCount) {
    bool pass;
    uint8_t status, chan, cc_mode, cc_dest, cc_end, cc_min, cc_max, cc_no_dups,
            cc_num, cc_start, cc_val, new_val, param_offset;
    int8_t cc_chan;
    uint8_t cc_src = (uint8_t) fParams[paramCCSource];
    struct MidiEvent cc_event;

    for (uint32_t i=0; i<eventCount; ++i) {
        pass = true;

        if ((status = events[i].data[0] & 0xF0) != MIDI_CONTROL_CHANGE) {
            writeMidiEvent(events[i]);
            continue;
        }

        chan = events[i].data[0] & 0x0F;
        cc_num = events[i].data[1] & 0x7f;

        if ((filterChannel == -1 || chan == filterChannel) && cc_num == cc_src) {
            pass = (bool) fParams[paramKeepOriginal];
            cc_val = events[i].data[2] & 0x7f;

            for (int dest=0; dest<4; dest++) {
                param_offset = paramCC1Mode + (8 * dest);
                cc_mode = (uint8_t) fParams[param_offset];
                cc_dest = (uint8_t) fParams[param_offset + 1];
                cc_chan = (int8_t) fParams[param_offset + 2] - 1;
                cc_no_dups = (bool) fParams[param_offset + 3];
                cc_start = (uint8_t) fParams[param_offset + 4];
                cc_end = (uint8_t) fParams[param_offset + 5];
                cc_min = (uint8_t) fParams[param_offset + 6];
                cc_max = (uint8_t) fParams[param_offset + 7];

                if (IN_RANGE(cc_val, cc_start, cc_end)) {
                    switch (cc_mode) {
                        case 1:
                            new_val = (uint8_t) MAP(cc_val, cc_start, cc_end, cc_min, cc_max);
                            break;
                        case 2:
                            new_val = (uint8_t) MAP(cc_val, 0, 127, cc_min, cc_max);
                            break;
                        default:
                            continue;
                    }

                    if (cc_chan == -1) {
                        cc_chan = chan;
                    }

                    if (cc_no_dups && new_val == lastCCValue[cc_chan][cc_dest])
                        continue;

                    cc_event.frame = events[i].frame;
                    cc_event.size = 3;
                    cc_event.data[0] = MIDI_CONTROL_CHANGE | cc_chan;
                    cc_event.data[1] = (uint8_t) cc_dest;
                    cc_event.data[2] = new_val;
                    lastCCValue[cc_chan][cc_dest] = new_val;
                    writeMidiEvent(cc_event);
                }
            }

            if (pass)
                writeMidiEvent(events[i]);
        }
        else {
            writeMidiEvent(events[i]);
        }
    }
}

// -----------------------------------------------------------------------

Plugin* createPlugin() {
    return new PluginMIDICCMapX4();
}

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO
