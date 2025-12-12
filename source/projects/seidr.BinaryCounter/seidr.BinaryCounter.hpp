/// @file       seidr.BinaryCounter.hpp
///	@ingroup 	seidr
///	@copyright	Copyright 2022 - Jóhann Berentsson. All rights reserved.
///	@license	Use of this source code is governed by the MIT License
///             found in the License.md file.

#pragma once

#include <c74_min.h>
#include "Counter/Counter.hpp"

using namespace c74::min;

class BinaryCounterMax : public object<BinaryCounterMax> {
public:
    MIN_DESCRIPTION {"Binary Counter"};    // NOLINT 
    MIN_TAGS        {"seidr"};             // NOLINT 
    MIN_AUTHOR      {"Jóhann Berentsson"}; // NOLINT 
    MIN_RELATED     {"seidr.*"};           // NOLINT 
    
    enum : std::uint8_t {
        OUTPUT_COUNT = 8
    };

    explicit BinaryCounterMax(const atoms &args = {});

    auto updateOutputs() -> void;
    auto getBit(int output) -> unsigned int;

    auto counterValue() -> unsigned int;
    auto setPreset(unsigned int presetValue) -> unsigned int;
    auto preset() -> unsigned int;
    auto maxValue() -> unsigned int;
    auto getStepCount() const -> int { return this->stepCount; };

    inlet<> input0 {this, "(bang | list | reset) input pulse"};
    inlet<> input1 {this, "(int | reset) reset pulse"};

    std::vector<std::unique_ptr<outlet<>>> outputs;

    message<threadsafe::yes> bang {
        this, "bang", "Steps the counter.",
        MIN_FUNCTION{
            switch(inlet){
                case 1:
                    this->counter_.reset();
                    this->alreadyBanged = false;

                    break;
                default:
                    if (this->alreadyBanged){
                        this->counter_.step();
                    } else {
                        this->alreadyBanged = true;
                    }
                    this->updateOutputs();
                    break;
            }
            return {};
        }
    };

    message<threadsafe::yes> reset {
        this, "reset", "Reset the counter.",
        MIN_FUNCTION{
            switch(inlet){
                case 1:
                    this->counter_.reset();
                    this->alreadyBanged = false;
                    break;
                default:
                    break;
            }
            return {};
        }
    };

    message<threadsafe::yes> preset_msg {
        this, "preset", "Set preset value.",
        MIN_FUNCTION{
            if (!args.empty() && inlet == 1) {
                int preset_value = args[0];
                this->counter_.setPreset(preset_value);
            } else if (args.empty() && inlet == 1) {
                this->counter_.preset();
            }
            return {};
        }
    };

    message<threadsafe::yes> output {
        this, "output", "Output current value without changing it.",
        MIN_FUNCTION{
            this->updateOutputs();
            return {};
        }
    };

    message<threadsafe::yes> max_value {this, "max", "Set the counter max value.",
        MIN_FUNCTION{
            if(!args.empty()){
                this->counter_.setMaxValue(static_cast<int> (args[0]));
            }
            return {};
        }
    };

    message<threadsafe::yes> bangEnable {this, "bangEnable", "Enable bang outputs.",
        MIN_FUNCTION{
            this->bangEnabled = true;
            return {};
        }
    };

    message<threadsafe::yes> bangDisable {this, "bangDisable", "Enable bang outputs.",
        MIN_FUNCTION{
            this->bangEnabled = false;
            return {};
        }
    };

private:
    Counter counter_;
    int stepCount = OUTPUT_COUNT;
    bool bangEnabled = false;
    bool alreadyBanged = false;
};
