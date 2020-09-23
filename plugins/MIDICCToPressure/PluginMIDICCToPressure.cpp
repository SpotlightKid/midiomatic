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

#include "PluginMIDICCToPressure.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

PluginMIDICCToPressure::PluginMIDICCToPressure()
    : Plugin(paramCount, presetCount, 0)  // 0 states
{
    loadProgram(0);
}

// -----------------------------------------------------------------------
// Init

void PluginMIDICCToPressure::initParameter(uint32_t index, Parameter& parameter) {
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
            parameter.name = "Keep original source CC events";
            parameter.shortName = "Keep original";
            parameter.symbol = "keep_original";
            parameter.hints |= kParameterIsBoolean;
            parameter.ranges.max = 1;
            break;
        case paramSrcCC:
            parameter.name = "Source CC";
            parameter.symbol = "src_cc";
            parameter.ranges.def = 1;
            break;
   }
}

/**
  Set the name of the program @a index.
  This function will be called once, shortly after the plugin is created.
*/
void PluginMIDICCToPressure::initProgramName(uint32_t index, String& programName) {
    if (index < presetCount) {
        programName = factoryPresets[index].name;
    }
}

// -----------------------------------------------------------------------
// Internal data

/**
  Optional callback to inform the plugin about a sample rate change.
*/
void PluginMIDICCToPressure::sampleRateChanged(double newSampleRate) {
    (void) newSampleRate;
}

/**
  Get the current value of a parameter.
*/
float PluginMIDICCToPressure::getParameterValue(uint32_t index) const {
    return fParams[index];
}

/**
  Change a parameter value.
*/
void PluginMIDICCToPressure::setParameterValue(uint32_t index, float value) {
    switch (index) {
        case paramFilterChannel:
            fParams[index] = CLAMP(value, 0.0f, 16.0f);
            filterChannel = (int8_t) fParams[index] - 1;
            break;
        case paramKeepOriginal:
            fParams[index] = CLAMP(value, 0.0f, 1.0f);
            break;
        case paramSrcCC:
            fParams[index] = CLAMP(value, 0.0f, 127.0f);
            break;
    }
}

/**
  Load a program.
  The host may call this function from any context,
  including realtime processing.
*/
void PluginMIDICCToPressure::loadProgram(uint32_t index) {
    if (index < presetCount) {
        for (int i=0; i < paramCount; i++) {
            setParameterValue(i, factoryPresets[index].params[i]);
        }

    }
}

// -----------------------------------------------------------------------
// Process

void PluginMIDICCToPressure::activate() {
    // plugin is activated
}


void PluginMIDICCToPressure::run(const float**, float**, uint32_t,
                                 const MidiEvent* events, uint32_t eventCount) {
    uint8_t chan;
    struct MidiEvent cc_event;
    bool pass = true;

    for (uint32_t i=0; i<eventCount; ++i) {
        if ((events[i].data[0] & 0xF0) != MIDI_CONTROL_CHANGE) {
            writeMidiEvent(events[i]);
            continue;
        }

        chan = events[i].data[0] & 0x0F;

        if ((filterChannel == -1 || chan == filterChannel) &&
            events[i].data[1] == (uint8_t) fParams[paramSrcCC] )
        {
            cc_event.frame = events[i].frame;
            cc_event.size = 2;
            cc_event.data[0] = MIDI_CHANNEL_PRESSURE | chan;
            cc_event.data[1] = events[i].data[2] & 0x7f;
            writeMidiEvent(cc_event);
            pass = (bool) fParams[paramKeepOriginal];
        }
        if (pass)
            writeMidiEvent(events[i]);
    }
}

// -----------------------------------------------------------------------

Plugin* createPlugin() {
    return new PluginMIDICCToPressure();
}

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO
