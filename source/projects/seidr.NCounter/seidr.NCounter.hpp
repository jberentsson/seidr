/// @file       seidr.NCounter.hpp
///	@ingroup 	seidr
///	@copyright	Copyright 2025 - Jóhann Berentsson. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include <vector>
#include <c74_min.h>
#include "Counter/Counter.hpp"

using namespace c74::min;

class NCounterMax : public object<NCounterMax> {
public:
    MIN_DESCRIPTION {"NCounter"};          // NOLINT 
    MIN_TAGS        {"seidr, counter"};    // NOLINT 
    MIN_AUTHOR      {"Jóhann Berentsson"}; // NOLINT 
    MIN_RELATED     {"seidr.*"};           // NOLINT 

    enum : uint8_t {
        OUTPUT_COUNT = 10
    };

    explicit NCounterMax(const atoms &args = {});

    void handleOutputs();
    auto counterValue() -> unsigned int;
    auto step() -> unsigned int;

    inlet<> input0{this, "(bang) input pulse"};
    inlet<> input1{this, "(int | reset | preset | preset_value) reset pulse"};

    std::vector<std::unique_ptr<outlet<>>> outputs;
    
    argument<symbol> bangArg{this, "bang_on", "Initial value for the bang attribute.", MIN_ARGUMENT_FUNCTION{bangEnabled_ = FALSE; }};

    message<threadsafe::yes> bang {this, "bang", "Steps the counter.",
        MIN_FUNCTION{
            if (this->alreadyBanged_){
                this->counter_.step();
            } else {
                this->alreadyBanged_ = true;
            }

            this->handleOutputs();
            return {};
        }
    };

    message<threadsafe::yes> reset {this, "reset", "Reset the counter.",
        MIN_FUNCTION{
            this->counter_.reset();
            this->alreadyBanged_ = false;
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

    message<threadsafe::yes> preset_value {this, "preset_value", "Set the counter preset value.",
        MIN_FUNCTION{
            if(!args.empty()){
                this->counter_.setPreset(static_cast<int> (args[0]));
            }

            return {};
        }
    };

    message<threadsafe::yes> preset {this, "preset", "Set the counter preset value.",
        MIN_FUNCTION{
            this->counter_.preset();
            return {};
        }
    };

    message<threadsafe::yes> bangEnable {this, "bangEnable", "Enable bang outputs.",
        MIN_FUNCTION{
            this->bangEnabled_ = true;
            return {};
        }
    };

    message<threadsafe::yes> bangDisable {this, "bangDisable", "Enable bang outputs.",
        MIN_FUNCTION{
            this->bangEnabled_ = false;
            return {};
        }
    };

private:
    Counter counter_;
    std::vector<int> outputStates_;
    bool bangEnabled_ = false;
    bool alreadyBanged_ = false;
    int stepCount_ = OUTPUT_COUNT;
};
