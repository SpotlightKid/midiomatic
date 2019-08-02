/*
 * MIDI XForm audio effect based on DISTRHO Plugin Framework (DPF)
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

#include "PluginMIDIXForm.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

PluginMIDIXForm::PluginMIDIXForm()
    : Plugin(paramCount, 12, 0)  // paramCount params, 12 program(s), 0 states
{
    loadProgram(0);
}

// -----------------------------------------------------------------------
// Init

void PluginMIDIXForm::initParameter(uint32_t index, Parameter& parameter) {
    if (index >= paramCount)
        return;

    ParameterEnumerationValue* const modes = new ParameterEnumerationValue[2];
    parameter.hints = kParameterIsAutomable | kParameterIsBoolean;
    parameter.ranges.def = 0.0f;
    parameter.ranges.min = 0.0f;
    parameter.ranges.max = 1.0f;

    switch (index) {
        case paramFilterMode:
            parameter.name = "Filter Mode";
            parameter.symbol = "filter_mode";
            parameter.hints = kParameterIsAutomable | kParameterIsInteger;
            modes[0].label = "Only block disabled";
            modes[0].value = 0;
            modes[1].label = "Pass only enabled";
            modes[1].value = 1;
            parameter.enumValues.count = 2;
            parameter.enumValues.restrictedMode = true;
            parameter.enumValues.values = modes;
            break;
        case paramSystemExclusive:
            parameter.name = " F0-SysEx";
            parameter.symbol = "sysex";
            break;
        case paramMTCQuarterFrame:
            parameter.name = "F1-MTC";
            parameter.symbol = "mtc_quarter_frame";
            break;
        case paramSongPositionPointer:
            parameter.name = "F2-SPP";
            parameter.symbol = "song_position_pointer";
            break;
        case paramSongSelect:
            parameter.name = "F3-SS";
            parameter.symbol = "song_select";
            break;
        case paramTuneRequest:
            parameter.name = "F6-Tune Request";
            parameter.symbol = "tune_request";
            break;
        case paramTimingClock:
            parameter.name = "F8-Clock";
            parameter.symbol = "timing_clock";
            break;
        case paramStart:
            parameter.name = "FA-Start";
            parameter.symbol = "start";
            break;
        case paramContinue:
            parameter.name = "FB-Continue";
            parameter.symbol = "continue";
            break;
        case paramStop:
            parameter.name = "FC-Stop";
            parameter.symbol = "stop";
            break;
        case paramActiveSensing:
            parameter.name = "FE-Active Sense";
            parameter.symbol = "active_sensing";
            break;
        case paramSystemReset:
            parameter.name = "FF-Reset";
            parameter.symbol = "system_reset";
            break;
        case paramUndefined:
            parameter.name = "Undefined";
            parameter.symbol = "undefined";
            break;
    }
}

/**
  Set the name of the program @a index.
  This function will be called once, shortly after the plugin is created.
*/
void PluginMIDIXForm::initProgramName(uint32_t index, String& programName) {
    switch (index) {
        case 0:
            programName = "Pass-through";
            break;
        case 1:
            programName = "Block any System message";
            break;
        case 2:
            programName = "Block System Exclusive";
            break;
        case 3:
            programName = "Block System Common";
            break;
        case 4:
            programName = "Block System Real-Time";
            break;
        case 5:
            programName = "Block Undefined";
            break;
        case 6:
            programName = "Block Active Sensing";
            break;
        case 7:
            programName = "Block Timing Clock";
            break;
        case 8:
            programName = "Pass System messages only";
            break;
        case 9:
            programName = "Pass System Exclusive only";
            break;
        case 10:
            programName = "Pass System Common only";
            break;
        case 11:
            programName = "Pass System Real-Time only";
            break;
    }
}

// -----------------------------------------------------------------------
// Internal data

/**
  Optional callback to inform the plugin about a sample rate change.
*/
void PluginMIDIXForm::sampleRateChanged(double newSampleRate) {
    (void) newSampleRate;
}

/**
  Get the current value of a parameter.
*/
float PluginMIDIXForm::getParameterValue(uint32_t index) const {
    return fParams[index];
}

/**
  Change a parameter value.
*/
void PluginMIDIXForm::setParameterValue(uint32_t index, float value) {
    fParams[index] = value;
}

/**
  Load a program.
  The host may call this function from any context,
  including realtime processing.
*/
void PluginMIDIXForm::loadProgram(uint32_t index) {
    switch (index) {
        case 0:  // Pass-through
            setParameterValue(paramFilterMode, 0.0f);
            for (int i=1; i<paramCount; i++) {
                setParameterValue(i, 1.0f);
            }
            break;
        case 1:  // Block any System messages
            setParameterValue(paramFilterMode, 0.0f);
            for (int i=1; i<paramCount; i++) {
                setParameterValue(i, 0.0f);
            }
            break;
        case 2:  // Block System Exclusive
            setParameterValue(paramFilterMode, 0.0f);
            for (int i=1; i<paramCount; i++) {
                setParameterValue(i, 1.0f);
            }
            setParameterValue(paramSystemExclusive, 0.0f);
            break;
        case 3:  // Block System Common
            setParameterValue(paramFilterMode, 0.0f);
            for (int i=1; i<paramCount; i++) {
                setParameterValue(i, 1.0f);
            }
            setParameterValue(paramMTCQuarterFrame, 0.0f);
            setParameterValue(paramSongPositionPointer, 0.0f);
            setParameterValue(paramSongSelect, 0.0f);
            setParameterValue(paramTuneRequest, 0.0f);
            break;
        case 4:  // Block System Real-Time
            setParameterValue(paramFilterMode, 0.0f);
            for (int i=1; i<paramCount; i++) {
                setParameterValue(i, 1.0f);
            }
            setParameterValue(paramTimingClock, 0.0f);
            setParameterValue(paramStart, 0.0f);
            setParameterValue(paramContinue, 0.0f);
            setParameterValue(paramStop, 0.0f);
            setParameterValue(paramActiveSensing, 0.0f);
            setParameterValue(paramSystemReset, 0.0f);
            break;
        case 5:  // Block Undefined
            setParameterValue(paramFilterMode, 0.0f);
            for (int i=1; i<paramCount; i++) {
                setParameterValue(i, 1.0f);
            }
            setParameterValue(paramUndefined, 0.0f);
            break;
        case 6:  // Block Active Sensing
            setParameterValue(paramFilterMode, 0.0f);
            for (int i=1; i<paramCount; i++) {
                setParameterValue(i, 1.0f);
            }
            setParameterValue(paramActiveSensing, 0.0f);
            break;
        case 7:  // Block Timing Clock
            setParameterValue(paramFilterMode, 0.0f);
            for (int i=1; i<paramCount; i++) {
                setParameterValue(i, 1.0f);
            }
            setParameterValue(paramTimingClock, 0.0f);
            break;
        case 8:  // Pass any System messages only
            setParameterValue(paramFilterMode, 1.0f);
            for (int i=1; i<paramCount; i++) {
                setParameterValue(i, 1.0f);
            }
            break;
        case 9:  // Pass System Exclusive only
            setParameterValue(paramFilterMode, 1.0f);
            for (int i=1; i<paramCount; i++) {
                setParameterValue(i, 0.0f);
            }
            setParameterValue(paramSystemExclusive, 1.0f);
            break;
        case 10:  // Pass System Common only
            setParameterValue(paramFilterMode, 1.0f);
            for (int i=1; i<paramCount; i++) {
                setParameterValue(i, 0.0f);
            }
            setParameterValue(paramMTCQuarterFrame, 1.0f);
            setParameterValue(paramSongPositionPointer, 1.0f);
            setParameterValue(paramSongSelect, 1.0f);
            setParameterValue(paramTuneRequest, 1.0f);
            break;
        case 11:  // Pass System Real-Time only
            setParameterValue(paramFilterMode, 1.0f);
            for (int i=1; i<paramCount; i++) {
                setParameterValue(i, 0.0f);
            }
            setParameterValue(paramTimingClock, 1.0f);
            setParameterValue(paramStart, 1.0f);
            setParameterValue(paramContinue, 1.0f);
            setParameterValue(paramStop, 1.0f);
            setParameterValue(paramActiveSensing, 1.0f);
            setParameterValue(paramSystemReset, 1.0f);
            break;
    }
}

// -----------------------------------------------------------------------
// Process

void PluginMIDIXForm::activate() {
    // plugin is activated
}



void PluginMIDIXForm::run(const float**, float**, uint32_t,
                          const MidiEvent* events, uint32_t eventCount) {
    bool pass;

    for (uint32_t i=0; i<eventCount; ++i) {

        if (events[i].size > MidiEvent::kDataSize &&
            events[i].dataExt[0] == MIDI_SYSTEM_EXCLUSIVE)
        {
            pass = (bool) fParams[paramSystemExclusive];
        }
        else {
            uint8_t status = events[i].data[0] & 0xFF;

            switch(status) {
                case MIDI_MTC_QUARTER_FRAME:
                    pass = (bool) fParams[paramMTCQuarterFrame];
                    break;
                case MIDI_SONG_POSITION_POINTER:
                    pass = (bool) fParams[paramSongPositionPointer];
                    break;
                case MIDI_SONG_SELECT:
                    pass = (bool) fParams[paramSongSelect];
                    break;
                case MIDI_TUNE_REQUEST:
                    pass = (bool) fParams[paramTuneRequest];
                    break;
                case MIDI_TIMING_CLOCK:
                    pass = (bool) fParams[paramTimingClock];
                    break;
                case MIDI_START:
                    pass = (bool) fParams[paramStart];
                    break;
                case MIDI_CONTINUE:
                    pass = (bool) fParams[paramContinue];
                    break;
                case MIDI_STOP:
                    pass = (bool) fParams[paramStop];
                    break;
                case MIDI_ACTIVE_SENSING:
                    pass = (bool) fParams[paramActiveSensing];
                    break;
                case MIDI_SYSTEM_RESET:
                    pass = (bool) fParams[paramSystemReset];
                    break;
                case MIDI_UNDEFINED_F4:
                case MIDI_UNDEFINED_F5:
                case MIDI_UNDEFINED_F9:
                case MIDI_UNDEFINED_FD:
                    pass = (bool) fParams[paramUndefined];
                    break;
                default:
                    pass = fParams[paramFilterMode] == 0 ? true : false;
            }
        }

        if (pass) writeMidiEvent(events[i]);
    }
}

// -----------------------------------------------------------------------

Plugin* createPlugin() {
    return new PluginMIDIXForm();
}

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO
