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

// #include <iostream>
#include <iomanip>
#include <vector>

#include "PluginMIDICCRecorder.hpp"
#include "extra/Base64.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

PluginMIDICCRecorder::PluginMIDICCRecorder()
    : Plugin(paramCount, presetCount, stateCount), playing(false)
{
    clearState();
    loadProgram(0);
}

// -----------------------------------------------------------------------
// Init

void PluginMIDICCRecorder::initParameter(uint32_t index, Parameter& parameter) {
    if (index >= paramCount)
        return;

    parameter.hints = kParameterIsAutomable | kParameterIsInteger;
    parameter.ranges.def = 0;
    parameter.ranges.min = 0;
    parameter.ranges.max = 1;

    switch (index) {
        case paramRecordEnable:
            parameter.name = "Record";
            parameter.symbol = "rec_enable";
            parameter.hints |= kParameterIsBoolean;
            break;
        case paramTrigClear:
            parameter.name = "Clear";
            parameter.symbol = "trig_clear";
            parameter.hints |= kParameterIsTrigger;
            break;
        case paramTrigSend:
            parameter.name = "Send";
            parameter.symbol = "trig_send";
            parameter.hints |= kParameterIsTrigger;
            break;
        case paramTrigTransport:
            parameter.name = "Trigger send on transport start?";
            parameter.shortName = "Transport";
            parameter.symbol = "trig_transport";
            parameter.ranges.max = 2;
            parameter.enumValues.count = 3;
            parameter.enumValues.restrictedMode = true;
            {
                ParameterEnumerationValue* const channels = new ParameterEnumerationValue[3];
                parameter.enumValues.values = channels;
                channels[0].label = "Disabled";
                channels[0].value = 0;
                channels[1].label = "Always";
                channels[1].value = 1;
                channels[2].label = "Only at Position 0";
                channels[2].value = 2;
            }
            break;
        case paramTrigPCChannel:
            parameter.name = "Trigger send on PC?";
            parameter.shortName = "Trigger by PC?";
            parameter.symbol = "trig_pc_chan";
            parameter.ranges.def = 17;
            parameter.ranges.max = 17;
            parameter.enumValues.count = 18;
            parameter.enumValues.restrictedMode = true;
            {
                ParameterEnumerationValue* const channels = new ParameterEnumerationValue[18];
                parameter.enumValues.values = channels;
                channels[0].label = "Any channel";
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
                channels[17].label = "Disabled";
                channels[17].value = 17;
            }
            break;
        case paramTrigPC:
            parameter.name = "Program number";
            parameter.symbol = "trig_pc";
            parameter.ranges.max = 127;
            break;
        case paramSendChannel:
            parameter.name = "Send channel";
            parameter.symbol = "send_chan";
            parameter.ranges.max = 16;
            parameter.enumValues.count = 17;
            parameter.enumValues.restrictedMode = true;
            {
                ParameterEnumerationValue* const channels = new ParameterEnumerationValue[17];
                parameter.enumValues.values = channels;
                channels[0].label = "All";
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
        case paramSendInterval:
            parameter.name = "Send interval (ms)";
            parameter.unit = "ms";
            parameter.symbol = "send_interval";
            parameter.ranges.def = 1;
            parameter.ranges.min = 1;
            parameter.ranges.max = 200;
            break;
   }
}

/**
  Set the state key and default value of @a index.
  This function will be called once, shortly after the plugin is created.
*/
void PluginMIDICCRecorder::initState(uint32_t index, String& stateKey, String& defaultStateValue) {
    if (index < stateCount) {
        char key[6];
        snprintf(key, 6, "ch-%02d", index);
        stateKey = key;
        defaultStateValue = "false";
    }
}

/**
  Set the name of the program @a index.
  This function will be called once, shortly after the plugin is created.
*/
void PluginMIDICCRecorder::initProgramName(uint32_t index, String& programName) {
    if (index < presetCount) {
        programName = factoryPresets[index].name;
    }
}

// -----------------------------------------------------------------------
// Internal data

/**
  Optional callback to inform the plugin about a sample rate change.
*/
void PluginMIDICCRecorder::sampleRateChanged(double newSampleRate) {
    fSampleRate = newSampleRate;
}

/**
  Get the current value of a parameter.
*/
float PluginMIDICCRecorder::getParameterValue(uint32_t index) const {
    return fParams[index];
}

/**
  Change a parameter value.
*/
void PluginMIDICCRecorder::setParameterValue(uint32_t index, float value) {
    switch (index) {
        case paramRecordEnable:
            fParams[index] = CLAMP(value, 0, 1);
            break;
        case paramTrigClear:
            fParams[index] = CLAMP(value, 0, 1);

            if (fParams[index] > 0.0f)
                clearState();

            break;
        case paramTrigSend:
            fParams[index] = CLAMP(value, 0, 1);

            if (fParams[index] > 0.0f)
                startSend();

            break;
        case paramTrigTransport:
            fParams[index] = CLAMP(value, 0, 2);
            break;
        case paramTrigPCChannel:
            fParams[index] = CLAMP(value, 0, 17);
            break;
        case paramTrigPC:
            fParams[index] = CLAMP(value, 0, 127);
            break;
        case paramSendChannel:
            fParams[index] = CLAMP(value, 0, 16);
            break;
        case paramSendInterval:
            fParams[index] = CLAMP(value, 0, 200);
            break;
    }
}

/**
  Get the value of an internal state.
  The host may call this function from any non-realtime context.
*/
String PluginMIDICCRecorder::getState(const char* key) const {
    static const String sFalse("false");
    int index;

    if (std::strncmp(key, "ch-", 3) == 0) {
        try {
            index = std::stoi(key+3);
        } catch (...) {
            return sFalse;
        }

        if (index < (int) stateCount) {
            return String::asBase64((char *) stateCC[index], 128);
        }
    }

    return sFalse;
}


/**
  Change an internal state.
*/
void PluginMIDICCRecorder::setState(const char* key, const char* value) {
    int index;

    if (std::strncmp(key, "ch-", 3) == 0) {
        try {
            index = std::stoi(key+3);
        } catch (...) {
            return;
        }

        if (index < (int) stateCount) {
            if (!std::strcmp(value, "false") == 0) {
                std::vector<uint8_t> state = d_getChunkFromBase64String(value);

                for (uint i=0; i < state.size(); i++) {
                    stateCC[index][i] = state[i];
                }

                // std::cerr << "State 'ch-" << index << "': ";
                // for (uint i=0; i < 128; i++) {
                //     std::cerr << (uint) stateCC[index][i] << ",";
                // }
                // std::cerr << std::endl;
            }
        }
    }
}


/**
  Clear all internal state.
*/
void PluginMIDICCRecorder::clearState() {
    for (uint i=0; i < 16; i++) {
        for (uint j=0; j < 128; j++) {
            stateCC[i][j] = 0xFF;
        }
    }
}

/**
  Load a program.
  The host may call this function from any context,
  including realtime processing.
*/
void PluginMIDICCRecorder::loadProgram(uint32_t index) {
    if (index < presetCount) {
        for (int i=0; i < paramCount; i++) {
            setParameterValue(i, factoryPresets[index].params[i]);
        }

    }
}

// -----------------------------------------------------------------------
// Process

/*
 *  Plugin is activated.
 */
void PluginMIDICCRecorder::activate() {
    fSampleRate = getSampleRate();
    sendInProgress = false;
    curChan = 0;
    curCC = 0;
}

/*
 *  Start sending.
 */
void PluginMIDICCRecorder::startSend() {
    if (!sendInProgress) {
        sendChannel = fParams[paramSendChannel];
        curChan = 0;
        curCC = 0;
    }

    sendInProgress = true;
}



void PluginMIDICCRecorder::run(const float**, float**, uint32_t nframes,
                               const MidiEvent* events, uint32_t eventCount) {
    uint8_t cc, chan, status;
    struct MidiEvent cc_event;
    bool block, start_send = false;
    static uint32_t next_frame = 0;

    const TimePosition& pos(getTimePosition());
    uint8_t trig_pc = (uint8_t) fParams[paramTrigPC];
    uint8_t trig_pc_chan = (uint8_t) fParams[paramTrigPCChannel];

    for (uint32_t i=0; i<eventCount; ++i) {
        block = false;
        status = events[i].data[0] & 0xF0;

        if (events[i].frame > next_frame) {
            next_frame = events[i].frame;
        }

        if (status >= 0xF0) {
            writeMidiEvent(events[i]);
            continue;
        }

        if (status < 0xF0) {
            chan = events[i].data[0] & 0x0F;

            if (status == MIDI_CONTROL_CHANGE) {
                if (sendInProgress && (sendChannel == 0 || sendChannel == chan + 1))
                    block = true;

                if (fParams[paramRecordEnable] && ! sendInProgress) {
                    cc = events[i].data[1] & 0x7F;
                    stateCC[chan][cc] = events[i].data[2] & 0x7F;
                }
            }
            else if (status == MIDI_PROGRAM_CHANGE &&
                     (trig_pc_chan == 0 || trig_pc_chan == chan + 1) &&
                      events[i].data[1] == trig_pc) {
                // trigger start of sending after MIDI events have been handled
                start_send = true;
            }
        }

        if (!block) writeMidiEvent(events[i]);
    }

    if (pos.playing and !playing) {
        playing = true;

        if (fParams[paramTrigTransport] == 1 ||
           (fParams[paramTrigTransport] == 2 && pos.frame == 0)) {
            startSend();
        }
    }
    else if (!pos.playing && playing) {
        playing = false;
    }

    if (start_send) {
        startSend();
    }

    if (sendInProgress) {
        bool sendOk = true;

        do {
            if (next_frame >= nframes) {
                next_frame -= nframes;
                break;
            }

            if ((sendChannel == 0 || sendChannel == curChan + 1) &&
                stateCC[curChan][curCC] != 0xFF)
            {
                cc_event.frame = next_frame;
                cc_event.size = 3;
                cc_event.data[0] = MIDI_CONTROL_CHANGE | (curChan & 0xF);
                cc_event.data[1] = curCC & 0x7F;
                cc_event.data[2] = stateCC[curChan][curCC] & 0x7F;
                sendOk = writeMidiEvent(cc_event);
                next_frame += cc_event.frame + (int) (fSampleRate / 1000 * fParams[paramSendInterval]);
            }

            curCC++;

            if (curCC >= NUM_CONTROLLERS) {
                curCC = 0;
                curChan++;

                if (curChan >= NUM_CHANNELS) {
                    curChan = 0;
                    sendInProgress = false;
                    next_frame = 0;
                    break;
                }
            }

        } while (sendOk);
    }
}

// -----------------------------------------------------------------------

Plugin* createPlugin() {
    return new PluginMIDICCRecorder();
}

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO
