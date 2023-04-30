//
//
// oscillator

#pragma once

#include "util.hpp"

enum class OSC_TYPE {
    SINE = 0,
    SQUARE,
    TRIANGLE,
};

class Osc {
private:
    float freq_;
    float phase_;
    float out_;

    const float inv_sample_rate_; 
    const OSC_TYPE type_;
public:
    Osc(float freq, double sample_rate, OSC_TYPE type = OSC_TYPE::SINE) : 
        freq_{freq}, phase_{0.}, out_{0.}, 
        inv_sample_rate_{static_cast<float>(1.0 / sample_rate)},
        type_{type} {
    }

    // increment phase
    void inc(float add_freq = 0.) {
        // generate required wave form at given phase
        switch(type_) {
            case OSC_TYPE::SINE: {
                out_ = sinf(phase_);
                break;
            }
            case OSC_TYPE::SQUARE: {
                if (phase_ < M_PI) {
                    out_ = 1;
                }
                else {
                    out_ = -1;
                }
                break;
            }
            case OSC_TYPE::TRIANGLE: {
                if (phase_ < M_PI_2) {
                    out_ = M_2_PI * phase_;
                }
                else if (M_PI_2 <= phase_ && phase_ < M_3PI_2) {
                    out_ = 2.0 - M_PI_2 * phase_;
                }
                else if (M_3PI_2 <= phase_ && phase_ < TAU) {
                    out_ = -4 + M_PI_2 * phase_;
                }
                break;
            }
            default:
                // should never get here
                break;
        }

        phase_ = phase_ + (add_freq+freq_)* M_2PI * inv_sample_rate_;

        while (phase_ >= M_2PI) {
            phase_ = phase_ - M_2PI;
        }
    }

    float out() const {
        return out_;
    }
};

