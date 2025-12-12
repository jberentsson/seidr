/// @file       seidr.Quantizer.hpp
///	@ingroup 	seidr
///	@copyright	Copyright 2025 - Jóhann Berentsson. All rights reserved.
///	@license	Use of this source code is governed by the MIT License
///             found in the License.md file.

#pragma once

#include "Quantizer/Quantizer.hpp"
#include <c74_min.h>

using namespace c74;

class QuantizerMax : public min::object<QuantizerMax> {
private:
    Quantizer quantizer_;

public:
    MIN_DESCRIPTION {"Quantize a MIDI note message."}; // NOLINT
    MIN_TAGS        {"seidr"};                         // NOLINT
    MIN_AUTHOR      {"Jóhann Berentsson"};             // NOLINT
    MIN_RELATED     {"seidr.*"};                       // NOLINT

    enum Inlets : uint8_t {
        NOTE = 0,
        ARGS = 1
    };

    using RoundDirection = Quantizer::RoundDirection;
    using QuantizeMode = Quantizer::QuantizeMode;
    using NoteThrough = Quantizer::NoteThrough;

    explicit QuantizerMax(const min::atoms &args = {});

    auto noteCount() -> int { return this->quantizer_.noteCount(); }
    auto getRoundDirection() -> RoundDirection { return this->quantizer_.getRoundDirection(); }
    auto processNoteMessage(int notePitch, int velocity) -> void;

    // Inlets
    min::inlet<> input_note       {this, "(list) note and velocity"};
    min::inlet<> input_arguments  {this, "(add|remove|update|mode|round|clear|through) input arguments"};

    // Outlets
    min::outlet<> output_note     {this, "(anything) output note"};
    min::outlet<> output_velocity {this, "(anything) output velocity"};
    min::outlet<> output_invalid  {this, "(bang) note was not playaed"};

    min::message<min::threadsafe::yes> anything {
        this, "anything", "Handle any input",
        MIN_FUNCTION {
            max::object_post(*this, "anything\n");
            return {};
        }
    };

    min::message<min::threadsafe::yes> integerInput {
        this, "int", "Handle integer input",
        MIN_FUNCTION {
            max::object_post(*this, "int\n");
            return {};
        }
    };

    min::message<min::threadsafe::yes> floatInput {
        this, "float", "Handle float input",
        MIN_FUNCTION {
            max::object_post(*this, "float\n");
            return {};
        }
    };

    min::message<min::threadsafe::yes> bangInput {
        this, "bang", "Handle bang input",
        MIN_FUNCTION {
            max::object_post(*this, "bang\n");
            return {};
        }
    };

    min::message<min::threadsafe::yes> list {
        this, "list", "Process note messages",
        MIN_FUNCTION {
            if (Inlets(inlet) == Inlets::NOTE && args.size() >= 2) {
                int note = static_cast<int>(args[0]);
                int velocity = static_cast<int>(args[1]);
                this->processNoteMessage(note, velocity);
            }
            
            return {};
        }
    };

    min::message<min::threadsafe::yes> quantizerAddNote {
        this, "add", "Add notes to quantizer",
        MIN_FUNCTION {
            if (Inlets(inlet) == Inlets::ARGS && !args.empty()) {
                for (const auto &arg : args) {
                    int note = static_cast<int>(arg);
                    if((note >= MIDI::RANGE_LOW) && (note <= MIDI::RANGE_HIGH) ) {
                        this->quantizer_.addNote(MIDI::Note(note));
                    }
                }                
            }
            
            return {};
        }
    };

    min::message<min::threadsafe::yes> quantizerThrough {
        this, "through", "Disable note through.",
        MIN_FUNCTION {
            if (Inlets(inlet) == Inlets::ARGS && !args.empty()) {
                int quantizeFlag = static_cast<int>(args[0]);
                this->quantizer_.setThrough(NoteThrough(quantizeFlag));
            }
            
            return {};
        }
    };

    min::message<min::threadsafe::yes> updateNotes {
        this, "update", "Clears all notes",
        MIN_FUNCTION {
            if (Inlets(inlet) == Inlets::ARGS && !args.empty()) {
                this->quantizer_.clear();
                
                for (const auto &argValue : args) {
                    int noteValue = static_cast<int>(argValue);
                    this->quantizer_.addNote(MIDI::Note(noteValue));
                }
            }

            return {};
        }
    };

    min::message<min::threadsafe::yes> quantizerClear {
        this, "clear", "Clear notes from the quantizer.",
        MIN_FUNCTION {
            if (Inlets(inlet) == Inlets::ARGS && !args.empty()) {
                this->quantizer_.clear();
            }

            return {};
        }
    };

    min::message<min::threadsafe::yes> quantizerMode {
        this, "mode", "Set quantizer mode.",
        MIN_FUNCTION {
            if (Inlets(inlet) == Inlets::ARGS && !args.empty()) {
                for (const auto &arg : args) {
                    int modeFlag = static_cast<int>(arg);
                    this->quantizer_.setMode(QuantizeMode(modeFlag));
                }                
            }
            
            return {};
        }
    };

    min::message<min::threadsafe::yes> quantizerRound {
        this, "round", "Set quantizer mode.",
        MIN_FUNCTION {
            if (Inlets(inlet) == Inlets::ARGS && !args.empty()) {
                for (const auto &arg : args) {
                    int modeFlag = static_cast<int>(arg);
                    this->quantizer_.setRoundDirection(RoundDirection(modeFlag));
                }
            }
            
            return {};
        }
    };

    min::message<min::threadsafe::yes> quantizerRange {
        this, "range", "Set quantizer range.",
        MIN_FUNCTION {
            if (Inlets(inlet) == Inlets::ARGS && !args.empty() && args.size() >= 2) {
                auto low = MIDI::Note(static_cast<int>(args[0]));
                auto high = MIDI::Note(static_cast<int>(args[1]));
                this->quantizer_.setRange(low, high);
            }
            
            return {};
        }
    };

    min::message<min::threadsafe::yes> quantizerDeleteNote {
        this, "delete", "Delete notes from quantizer",
        MIN_FUNCTION {
            if (Inlets(inlet) == Inlets::ARGS && !args.empty()){
                for(const auto &arg : args){
                    this->quantizer_.deleteNote(MIDI::Note(static_cast<int>(arg)));
                }
            }
            
            return {};
        }
    };
};
