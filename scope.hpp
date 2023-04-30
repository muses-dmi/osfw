//
//
// scope

#pragma once

#include <memory>

#include "lfspsc_queue.hpp"

class Scope {
private:
    int x_;
    int y_;
    int width_;
    int height_;
    size_t buffer_size_;
    std::shared_ptr<lfspsc_queue<float>> buffer_;
public:
    Scope() :
       x_{0}, y_{0}, width_{0}, height_{0}, buffer_size_{0}, buffer_{nullptr} {
    }

    Scope(
        int x, int y, 
        int width, int height, 
        unsigned int num_frames, 
        std::shared_ptr<lfspsc_queue<float>> buffer) :
        x_{x}, y_{y}, 
        width_{width}, height_{height}, 
        buffer_size_{num_frames}, 
        buffer_{buffer} {
    }

    ~Scope() {
    }

    void draw(NVGcontext* vg) const {

        // draw rectangle to give the scope shape        
        nvgBeginPath(vg);
        nvgRect(vg, x_, y_, width_, height_);
        nvgStrokeColor(vg, nvgRGBA(255,255,255,255));

        for (int i = 0; i < width_; i++) {
            
            float amp = 1.;
            buffer_->pop(amp);

            // first scale 
            int h2 = height_ / 2;
            amp = amp * h2;

            // now draw pos and neg amp values
            if (amp >= 0.) {
                nvgMoveTo(vg, x_+ i, y_ + h2);
                nvgLineTo(vg, x_+ i, (y_ + h2) - static_cast<int>(amp));
            }
            else {
                nvgMoveTo(vg, x_+ i, y_ + h2);
                nvgLineTo(vg, x_+ i, (y_ + h2) + static_cast<int>(amp) * -1);
            }
        }

        nvgStroke(vg);
    }
};