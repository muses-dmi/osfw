//
//
// oscillator

#pragma once

class Osc {
private:
    float freq_;
    float phase_;
    float out_;

    const float tau_;
    const float sample_rate_;
public:
    Osc(float freq, float tau, float sample_rate) : 
        freq_{freq}, phase_{0.}, out_{0.}, tau_{tau}, sample_rate_{sample_rate} {

    }

    // increment phase
    void inc(float add_freq = 0.) {
        phase_ += (add_freq+freq_)*tau_/sample_rate_;

        if(phase_ >= tau_) {
            phase_ -= tau_;
        }

        out_ = sinf(phase_);
    }

    float out() const {
        return out_;
    }
};

