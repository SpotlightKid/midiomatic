# Ideas for new plug-ins and features


## New plugins

### Note to Program Change / Bank Select

Sends Bank Select LSB and/or MSB (optional) and Program Change triggered
by a received Note On or Note Off in a specified range.

* Input Channel (Any, 1..16)
* Keep other events
* Source Note Min
* Source Note Max
* Destination:
  * Channel (Source, 1..16)
  * Program Change (None, 0..127)
  * Bank MSB Select (None, 0..127)
  * Bank LSB Select (None, 0..127)


### Program Change Trigger x 8

Send up to 8 Bank Select MSB/LSB and PC messages triggered by a
received PC message, via trigger input or transport start.

Params:

* Input Channel (Any, 1..16)
* Keep other events
* Source PC
* Send Trigger
* PC #1
  * Channel (Source, 1..16)
  * Program Change (None, 0..127)
  * Bank MSB Select (None, 0..127)
  * Bank LSB Select (None, 0..127)
* PC #2
  * ...

### Program Change to CC x 8

Convert PC messages into up to 8 CCs

Params:

* Input Channel (Any, 1..16)
* Keep other events
* Source PC
* Destination CC #1
  * CC (None, 0..127)
  * Channel (1..16)
  * Value (0..127)
* Destination CC #2
  * ...

### CC To Program Change x 8

Convert a CC message into up to 8 PCs

Params:

* Input Channel (Any, 1..16)
* Keep other events
* Source CC
* Destination PC A
  * PC A "on" (Disabled, 0..127)
  * Channel (1..16)
  * PC A "off" (Disabled, 0..127)
  * Channel (1..16)
  * On/Off value threshold (0..127)
* Destination CC #2
  * ...


### (N)RPN to CC

Converts (N)RPN data into CC messages


### CC To Pressure

As the reverse to MIDI Pressure To CC

Done.


### MIDI Polyphony

Route notes to different MIDI channels depending on "voice".


### PitchBend Enforce Scale

Scale PitchBend dynamically according to specified musical scale and last
played note.

Would only work properly with monophonic material or MPE.


## New Features

### MIDI CC Recorder

* Trigger via PC - done
* Trigger via Transport - done


## Changes

### Midi CC Map X4

* Rename CC 1..4 to CC A/B/C/D - no - would lead to short labels like CCA,CCB,CCC,CCD
