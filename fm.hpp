//
//
//

#pragma once

#include "osc.hpp"

class FM {
private:
    Osc mod_;
    Osc car_;
public:
    FM(Osc mod, Osc car) 
        : mod_{mod}, car_{car} {
    }

    void step(float amp) {
        mod_.inc();
        car_.inc(mod_.out()*amp);
    }

    float out() const {
        return car_.out();
    }
};