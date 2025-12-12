/// @file       seidr.Chords.hpp
///	@ingroup 	seidr
///	@copyright	Copyright 2025 - Jóhann Berentsson. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include "Chords/Chords.hpp"
#include <c74_min.h>

using namespace c74;

constexpr int NOTE_OFF = 0;
constexpr int NOTE_ON = 127;

class ChordsMax : public min::object<ChordsMax> {
public:
    MIN_DESCRIPTION { "Chords" };            // NOLINT 
    MIN_TAGS        { "seidr" };             // NOLINT 
    MIN_AUTHOR      { "Jóhann Berentsson" }; // NOLINT 
    MIN_RELATED     { "seidr.*" };           // NOLINT 

    enum Inlets : uint8_t { // NOLINT
        NOTES = 0,
        ARGS = 1
    };

    enum Outlets : uint8_t {
        NOTE = 0,
        VELOCITY = 1,
        OTHER = 2
    };

    using NoteMode = Chords::NoteMode;
    using NoteOrder = Chords::NoteOrder;

    min::inlet<> input_notes           { this, "(list) note velocity" };
    min::inlet<> input_args            { this, "(clear|record|set) arguments" };
    
    min::outlet<> output_note          { this, "(anything) output note" };
    min::outlet<> output_velocity      { this, "(anything) output velocity" };
    min::outlet<> output_note_velocity { this, "(anything) output note and velocity" };
    
    ChordsMax(const min::atoms &args = {}) {};

    auto getNoteMode() -> Chords::NoteMode {
        return this->chords_.getNoteMode();
    }
    
    auto getNoteOrder() -> Chords::NoteOrder {
        return this->chords_.getNoteOrder();
    }

    min::message<min::threadsafe::yes> floatInput {this, "float", "Recive note input.",
        MIN_FUNCTION{
            max::object_post(*this, "inlet ", inlet);
            return this->listInput(args);
        }
    };

    min::message<min::threadsafe::yes> intInput {this, "int", "Recive note input.",
        MIN_FUNCTION{
            max::object_post(*this, "inlet ", inlet);
            return this->listInput(args);
        }
    };

    min::message<min::threadsafe::yes> listInput {this, "list", "Recive note input.",
        MIN_FUNCTION{
            if(!args.empty() && (args.size() == 2) && (inlet == 0) && this->inputEnabled_) {
                int pitchValue = static_cast<int>(args[0]);
                int velocityValue = static_cast<int>(args[1]);

                if(this->chordsEnabled_) {
                    // If the chord memory is enabled.
                    this->chords_.note(pitchValue, velocityValue);

                    // Send out the notes on the note queue.
                    for(const auto &currentNote : this->chords_.noteQueue()) {
                        output_velocity.send(static_cast<int>(currentNote->velocity()));
                        output_note.send(static_cast<int>(currentNote->pitch()));
                        output_note_velocity.send({
                            static_cast<int>(currentNote->pitch()),
                            static_cast<int>(currentNote->velocity())
                        });

                        max::object_post(*this, "--- note sent %d %d\n", MIDI::human(currentNote->pitch()), MIDI::human(currentNote->velocity()));
                    }

                    this->chords_.noteQueue().clear();
                } else {
                    // Send the notes through if the chord memory has been disabled.
                    output_velocity.send(velocityValue);
                    output_note.send(pitchValue);
                    output_note_velocity.send({
                        pitchValue,
                        velocityValue
                    });
                }
            }

            return {};
        }
    };

    min::message<min::threadsafe::yes> recordNotes {this, "record", "record",
        MIN_FUNCTION{
            // Record notes for a specific key.

            if (inlet == 1) {
                max::object_post(*this, "record\n");
                this->chords_.reciveNotes();
            }

            return {};
        }
    };

    min::message<min::threadsafe::yes> enableChords {this, "enable", "enable the object",
        MIN_FUNCTION{
            // Enable the chord mode.

            if (inlet == 1) {
                this->chordsEnabled_ = true;
            }

            return {};
        }
    };

    min::message<min::threadsafe::yes> disableChords {this, "disable", "disable the object",
        MIN_FUNCTION{
            // Disable the chord mode and allow notes to go through.

            if (inlet == 1) {
                this->chordsEnabled_ = false;;
            }

            return {};
        }
    };

    min::message<min::threadsafe::yes> setKey {this, "set", "assign notes to a key",
        MIN_FUNCTION{
            // Set the notes for a specific key.

            if (inlet == 1 && !args.empty() && args.size() >= 3) {
                this->inputEnabled_ = false;
                int key = static_cast<int>(args[0]);

                this->chords_.clear(key);
                this->chords_.reciveNotes();
                
                for (int i = 0; i < args.size(); i++) {
                    int note = static_cast<int>(args[i]);

                    this->chords_.note(note, NOTE_ON);

                    if (i == 0) {
                        this->chords_.note(note, NOTE_OFF);
                    }
                }

                for (int i = 1; i < args.size(); i++) {
                    int note = static_cast<int>(args[i]);
                    this->chords_.note(note, NOTE_OFF);
                }

                this->inputEnabled_ = true;
            }

            return {};
        }
    };

    min::message<min::threadsafe::yes> clearKey {this, "clear", "clear notes from a key",
        MIN_FUNCTION{
            // Clear notes from all or a specific keys.

            if (inlet == 1) {
                this->inputEnabled_ = false;

                if (args.empty()) {
                    this->chords_.clear();
                    max::object_post(*this, "clear all\n");
                } else {
                    int keyValue = static_cast<int>(args[1]);
                    this->chords_.clear(keyValue);
                    max::object_post(*this, "clear %d\n", keyValue);
                }

                this->inputEnabled_ = true;
            }

            return {};
        }
    };

    min::message<min::threadsafe::yes> noteMode {this, "mode", "0 = retrigger, 1 = legato",
        MIN_FUNCTION{
            // Set the note mode.
            if (inlet == 1 && !args.empty()) {
                auto newMode = Chords::NoteMode(static_cast<int>(args[1]));
                max::object_post(*this, "--------------------------- note mode %d\n", newMode);
                newMode = this->chords_.setNoteMode(newMode);
                max::object_post(*this, "--------------------------- note mode %d\n", newMode);
            }

            return {};
        }
    };

    min::message<min::threadsafe::yes> noteOrder {this, "order", "0 = as played, 1 = random",
        MIN_FUNCTION{
            // Set the note order.
            if (inlet == 1 && !args.empty()) {
                auto newOrder =  Chords::NoteOrder(static_cast<int>(args[1]));
                max::object_post(*this, "--------------------------- note order %d\n", newOrder);
                newOrder = this->chords_.setNoteOrder(newOrder);
                max::object_post(*this, "--------------------------- note order %d\n", newOrder);
            }

            return {};
        }
    };

private:
    Chords chords_;
    
    bool chordsEnabled_ = true;
    bool inputEnabled_ = true;
};
