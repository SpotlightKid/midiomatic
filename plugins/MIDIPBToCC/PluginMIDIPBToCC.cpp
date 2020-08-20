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

#include "PluginMIDIPBToCC.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

PluginMIDIPBToCC::PluginMIDIPBToCC()
    : Plugin(paramCount, presetCount, 0)  // 0 states
{
    loadProgram(0);
}

// -----------------------------------------------------------------------
// Init

void PluginMIDIPBToCC::initParameter(uint32_t index, Parameter& parameter) {
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
            parameter.ranges.def = 0;
            parameter.ranges.min = 0;
            parameter.ranges.max = 16;
            parameter.enumValues.count = 17;
            parameter.enumValues.restrictedMode = true;
            {
                ParameterEnumerationValue* const channels = new ParameterEnumerationValue[17];
                parameter.enumValues.values = channels;
                channels[0].label = "Any";
                channels[0].value = 0;
                channels[1].label = "Channel 1";
                channels[1].value = 1;
                channels[2].label = "Channel 2";
                channels[2].value = 2;
                channels[3].label = "Channel 3";
                channels[3].value = 3;
                channels[4].label = "Channel 4";
                channels[4].value = 4;
                channels[5].label = "Channel 5";
                channels[5].value = 5;
                channels[6].label = "Channel 6";
                channels[6].value = 6;
                channels[7].label = "Channel 7";
                channels[7].value = 7;
                channels[8].label = "Channel 8";
                channels[8].value = 8;
                channels[9].label = "Channel 9";
                channels[9].value = 9;
                channels[10].label = "Channel 10";
                channels[10].value = 10;
                channels[11].label = "Channel 11";
                channels[11].value = 11;
                channels[12].label = "Channel 12";
                channels[12].value = 12;
                channels[13].label = "Channel 13";
                channels[13].value = 13;
                channels[14].label = "Channel 14";
                channels[14].value = 14;
                channels[15].label = "Channel 15";
                channels[15].value = 15;
                channels[16].label = "Channel 16";
                channels[16].value = 16;
            }
            break;
        case paramKeepOriginal:
            parameter.name = "Keep original PB events";
            parameter.shortName = "Keep PB";
            parameter.symbol = "keep_original";
            parameter.hints |= kParameterIsBoolean;
            parameter.ranges.max = 1;
            break;
        case paramPBMin:
            parameter.name = "PB min. value";
            parameter.symbol = "pb_min";
            parameter.ranges.def = -8192;
            parameter.ranges.min = -8192;
            parameter.ranges.max = 8191;
            break;
        case paramPBMax:
            parameter.name = "PB max. value";
            parameter.symbol = "pb_max";
            parameter.ranges.def = -8192;
            parameter.ranges.min = -8192;
            parameter.ranges.max = 8191;
            break;
        case paramCC1:
            parameter.name = "Pos. PB -> CC A";
            parameter.symbol = "cc1";
            parameter.ranges.def = 1;
            break;
        case paramCC1Min:
            parameter.name = "CC A min. value";
            parameter.symbol = "cc1_min";
            break;
        case paramCC1Max:
            parameter.name = "CC A max. value";
            parameter.symbol = "cc1_max";
            parameter.ranges.def = 127;
            break;
        case paramCC2:
            parameter.name = "Neg. PB -> CC B";
            parameter.symbol = "cc2";
            parameter.ranges.def = 1;
            break;
        case paramCC2Min:
            parameter.name = "CC B min. value";
            parameter.symbol = "cc2_min";
            break;
        case paramCC2Max:
            parameter.name = "CC B max. value";
            parameter.symbol = "cc2_max";
            parameter.ranges.def = 127;
            break;
   }
}

/**
  Set the name of the program @a index.
  This function will be called once, shortly after the plugin is created.
*/
void PluginMIDIPBToCC::initProgramName(uint32_t index, String& programName) {
    if (index < presetCount) {
        programName = factoryPresets[index].name;
    }
}

// -----------------------------------------------------------------------
// Internal data

/**
  Optional callback to inform the plugin about a sample rate change.
*/
void PluginMIDIPBToCC::sampleRateChanged(double newSampleRate) {
    (void) newSampleRate;
}

/**
  Get the current value of a parameter.
*/
float PluginMIDIPBToCC::getParameterValue(uint32_t index) const {
    return fParams[index];
}

/**
  Change a parameter value.
*/
void PluginMIDIPBToCC::setParameterValue(uint32_t index, float value) {
    switch (index) {
        case paramFilterChannel:
            fParams[index] = CLAMP(value, 0.0f, 16.0f);
            filterChannel = (int8_t) fParams[index] - 1;
            break;
        case paramKeepOriginal:
            fParams[index] = CLAMP(value, 0.0f, 1.0f);
            break;
        case paramPBMin:
        case paramPBMax:
            fParams[index] = CLAMP(value, -8192.0f, 8191.0f);
            break;
        case paramCC1:
        case paramCC1Min:
        case paramCC1Max:
        case paramCC2:
        case paramCC2Min:
        case paramCC2Max:
            fParams[index] = CLAMP(value, 0.0f, 127.0f);
            break;
    }
}

/**
  Load a program.
  The host may call this function from any context,
  including realtime processing.
*/
void PluginMIDIPBToCC::loadProgram(uint32_t index) {
    if (index < presetCount) {
        for (int i=0; i < paramCount; i++) {
            setParameterValue(i, factoryPresets[index].params[i]);
        }

    }
}

// -----------------------------------------------------------------------
// Process

void PluginMIDIPBToCC::activate() {
    // plugin is activated
}


void PluginMIDIPBToCC::run(const float**, float**, uint32_t,
                           const MidiEvent* events, uint32_t eventCount) {
    bool pass;
    uint8_t chan;
    int16_t pb_value,
            pb_min = (uint16_t) fParams[paramPBMin],
            pb_max = (uint16_t) fParams[paramPBMax];
    struct MidiEvent cc_event;

    for (uint32_t i=0; i<eventCount; ++i) {
        pass = true;

        if ((events[i].data[0] & 0xF0) != MIDI_PITCH_BEND) {
            writeMidiEvent(events[i]);
            continue;
        }

        chan = events[i].data[0] & 0x0F;

        if (filterChannel == -1 || chan == filterChannel) {
            pb_value = (((events[i].data[2] & 0x7f) << 7) | (events[i].data[1] & 0x7f)) - 8192;

            if (IN_RANGE(pb_value, pb_min, pb_max)) {
                pass = (bool) fParams[paramKeepOriginal];
                cc_event.frame = events[i].frame;
                cc_event.size = 3;
                cc_event.data[0] = MIDI_CONTROL_CHANGE | chan;

                if (pb_value >= 0) {
                    cc_event.data[1] = (uint8_t) fParams[paramCC1];

                    if (pb_min <= pb_max)
                        cc_event.data[2] = ((uint8_t) MAP(pb_value, 0, pb_max, fParams[paramCC1Min], fParams[paramCC1Max])) & 0x7f;
                    else
                        cc_event.data[2] = ((uint8_t) MAP(pb_value, pb_min, 8191, fParams[paramCC2Min], fParams[paramCC1Max])) & 0x7f;
                }
                else {
                    cc_event.data[1] = (uint8_t) fParams[paramCC2];

                    if (pb_min <= pb_max)
                        cc_event.data[2] = ((uint8_t) MAP(pb_value, -1, pb_min, fParams[paramCC2Min], fParams[paramCC2Max])) & 0x7f;
                    else
                        cc_event.data[2] = ((uint8_t) MAP(pb_value, pb_max, -8192, fParams[paramCC2Min], fParams[paramCC2Max])) & 0x7f;
                }

                writeMidiEvent(cc_event);
            }
        }

        if (pass) writeMidiEvent(events[i]);
    }
}

// -----------------------------------------------------------------------

Plugin* createPlugin() {
    return new PluginMIDIPBToCC();
}

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO
