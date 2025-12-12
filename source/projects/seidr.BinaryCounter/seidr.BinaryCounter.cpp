/// @file       seidr.BinaryCounter.cpp
///	@ingroup 	seidr
///	@copyright	Copyright 2022 - Jóhann Berentsson. All rights reserved.
///	@license	Use of this source code is governed by the MIT License
///             found in the License.md file.

#include "seidr.BinaryCounter.hpp"

BinaryCounterMax::BinaryCounterMax(const atoms &args) {
    if (!args.empty()) {
        this->stepCount = args[0];
    }

    // Create outputs
    for (int i = 0; i < this->stepCount; i++) {
        outputs.push_back(
            std::make_unique<outlet<>>(this, "(anything) output bit " + std::to_string(i)));
    }

    int calcucatedMaxValue = (int) std::pow(2, this->stepCount - 1);
    this->counter_ = Counter(calcucatedMaxValue);

    updateOutputs();
}

auto BinaryCounterMax::getBit(int output) -> unsigned int {
    return ((this->counter_.value()) >> output) & 0x1;
}

void BinaryCounterMax::updateOutputs() {
    // Send data to all outputs
    for (int i = 0; i < OUTPUT_COUNT; i++) {
        int current = OUTPUT_COUNT - i - 1;

        if (this->bangEnabled) {
            if (this->getBit(i) == 1) {
                this->outputs[current]->send("bang");
            }
        } else {
            this->outputs[current]->send(this->getBit(i));
        }
    }
}

/* void BinaryCounterMax::enableBangs() {
    this->bangEnabled = true;
    this->updateOutputs();
}

void BinaryCounterMax::disableBangs() {
    this->bangEnabled = false;
    this->updateOutputs();
} */

auto BinaryCounterMax::counterValue() -> unsigned int {
    return this->counter_.value();
}

auto BinaryCounterMax::setPreset(unsigned int presetValue) -> unsigned int {
    unsigned int result = this->counter_.setPreset(presetValue);
    this->updateOutputs();
    return result;
}

auto BinaryCounterMax::preset() -> unsigned int {
    return this->counter_.preset();
}

auto BinaryCounterMax::maxValue() -> unsigned int {
    return this->counter_.getMaxValue();
}

MIN_EXTERNAL(BinaryCounterMax); // NOLINT
