/// @file       seidr.Waves.hpp
///	@ingroup 	seidr
///	@copyright	Copyright 2025 - Jóhann Berentsson. All rights reserved.
///	@license	Use of this source code is governed by the MIT License
///             found in the License.md file.

#pragma once

#include <c74_min.h>
#include <memory>

using namespace c74;

class seidrWaves : public min::object<seidrWaves> {
private:
    int frequency;
    std::vector<std::shared_ptr<min::lib::oscillator<>>> waves;

    auto testing() -> void {
        waves.push_back(std::make_shared<min::lib::oscillator<min::lib::generator::sine<>>>());
    }

public:
    MIN_DESCRIPTION {"Quantize a MIDI note message."}; // NOLINT
    MIN_TAGS        {"seidr"};                         // NOLINT
    MIN_AUTHOR      {"Jóhann Berentsson"};             // NOLINT
    MIN_RELATED     {"seidr.*"};                       // NOLINT

    min::inlet<> in0 {this, "(number) frequency"};
    min::outlet<> out0 {this, "(signal) sound wave"};

    min::argument<min::number> frequency_arg { this, "frequency", "Initial frequency in hertz.",
        MIN_ARGUMENT_FUNCTION {
            this->frequency = arg;
        }
    };
};
