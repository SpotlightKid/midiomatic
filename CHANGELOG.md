ChangeLog
=========

0.2.0 (2021-01-15)
------------------

Features:

* Added `MIDIPresuureToCC` plugin to turn aftertouch into CC (contributed by Jorik Jinker).
* Added `MIDICCToPressure` plugin to turn a CC into aftertouch.
* Added `MIDICCMAPX4` plugin to turn a CC into up to four others.
* `MIDICCRecorder`: added support for activating send trigger by Program Change MIDI message.
* `MIDICCRecorder`: added support for activating send trigger by transport start.
* `MIDIPBToCC`: added 'Modulation +' preset.

Fixes:

* `MIDIPBToCC`: fixed sign of default 'pb_max' parameter value.
* Several small readme corrections.

Changes:

* `MIDICCRecorder`: tweaked some parameter labels and LV2 symbols.
* `MIDICCRecorder` is considered *beta* quality.
* Moved plugin descriptions and screenshots from readme into separate document.
* Updated plugin descriptions, screenshots and authors section in readme.
* Updated dpf git submodule to commit 27d3046.


0.1.1-beta (2020-08-21)
-----------------------

Fixes:

* `MIDIPBToCC`: fixed a parameter label.
* `MIDICCRecorder`: used proper value range for `sendChannel`.
* `MIDIPBToCC`: fixed `filter_channel` param internal state.

Changes:

* Switched dpf git submodule to develop branch and updated it.


0.1.0-beta2 (2019-12-15)
------------------------

Fixes:

* Fixed `install` make target.


0.1.0-beta1 (2019-10-15)
------------------------

First beta status release.

* `MIDISysFilter` is considered *stable*.
* `MIDIPBToCC` is considered *stable*.
* `MIDICCRecorder` is considered *experimental*.


0.1.0-alpha (2019-08-13)
------------------------

First alpha release
