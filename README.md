# Seidr

## Build Status
[![Developent](https://github.com/jberentsson/seidr/actions/workflows/main.yml/badge.svg?brach=develop&label=Tests)](https://github.com/jberentsson/seidr/actions/workflows/main.yml) [![Main](https://github.com/jberentsson/seidr/actions/workflows/main.yml/badge.svg?brach=main&label=Main)](https://github.com/jberentsson/seidr/actions/workflows/main.yml)

## Description
This package was created using the Min-DevKit for Max, an API and supporting tools for writing externals in modern C++. This project is still in a alpha stage so there are still known and unknown bugs in the objects. The objects are not all in their final form so they may change.

## Objects

### seidr.BinaryCounter
Binary counter.

### seidr.Chords
Chord memory where you can assign chords to notes on the fly.

### seidr.NCounter
Basic counter.

### seidr.RandomNoteOctave
Randomizes the octave of a MIDI note.

### seidr.ShiftRegister
Works like an binary shift register except it can handle more than juts binary values.

### seidr.Quantizer
Quantizer with a twist. It can operate like a normal quantizer where all notes of the keyboard are quantized to a specific scale but there is also a second mode.

In the second mode the object uses the whole keyboard as a scale.

## Build
```bash
# Setup CMake.
cmake -B build

# Build the project.
cmake --build build --config Release

# Or if you want fresh build.
cmake --build build --config Release --clean-first

# Or if you want fresh build.
cmake --build build --config Release --clean-first --target <TARGET_NAME>
```

## Available Targets
### Projects:
- seidr.BinaryCounter
- seidr.BinaryCounter_test
- seidr.Chords
- seidr.Chords_test
- seidr.NCounter
- seidr.NCounter_test
- seidr.RandomNoteOctave
- seidr.RandomNoteOctave_tess
- seidr.ShiftRegister
- seidr.Quantizer
- seidr.Quantizer_test

### Libraries:
- BinaryCounter
- BinaryCounter_test
- Chords
- Chords_test
- Counter
- Counter_test
- RandomOctave
- RandomOctave_test
- RandomNoteOctave
- RandomNoteOctave_test
- ShiftRegister
- ShiftRegister_test
