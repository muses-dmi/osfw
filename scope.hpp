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
    float sample_rate_;
    size_t buffer_size_;
    std::shared_ptr<lfspsc_queue<float>> buffer_;

    int current_offset_;
    int *data_;
    int framerate_;
    int ht2_;

    int count_;
    bool freeze_;
public:
    Scope() :
       x_{0}, y_{0}, 
       width_{0}, height_{0}, 
       sample_rate_{0.},
       buffer_size_{0}, buffer_{nullptr},
       current_offset_{0}, data_{nullptr},
       framerate_{0},
       ht2_{0},
       count_{0},
       freeze_{false} {
    }

    Scope(
        int x, int y, 
        int width, int height, 
        float sample_rate,
        unsigned int num_frames, 
        std::shared_ptr<lfspsc_queue<float>> buffer,
        int framerate) :
        x_{x}, y_{y}, 
        width_{width}, height_{height}, 
        sample_rate_{sample_rate},
        buffer_size_{num_frames}, buffer_{buffer},
        current_offset_{0}, data_{new int[width]},
        framerate_{framerate},
        ht2_{height_ / 2},
        count_{0},
        freeze_{false} {
            for (int i = 0; i < width; i++) {
                data_[i] = 0;
            }
    }

    ~Scope() {
    }

    void freeze() {
        freeze_ = !freeze_;
    }

    void draw(NVGcontext* vg) {

        // draw rectangle to give the scope shape        
        nvgBeginPath(vg);
        nvgRect(vg, x_, y_, width_, height_);
        nvgStrokeColor(vg, nvgRGBA(255,255,255,255));

        if ( !freeze_) {
            for (int i = 0; i < width_ - buffer_size_; i++) {
                // copy previous data to left by buffer size
                data_[i] = data_[i+buffer_size_];
            }

            for (int i = width_ - buffer_size_; i < width_; i++) {
                
                float amp = 0.;
                buffer_->pop(amp);

                // first scale 
                int h2 = height_ / 2;
                amp = amp * h2;
                data_[i] = amp;
            }
        }
        count_++;
        if (count_ == 6) {
            count_ = 0;
        }


        for (int i = 0; i < width_; i++) {
            nvgMoveTo(vg, x_+ i, y_ + ht2_);
            nvgLineTo(vg, x_+ i, (y_ + ht2_) + data_[i] * -1);
        }

    // sample_rate_ / buffer_size = 171.875

    // (sample_rate / buffer_size) / framerate_ = 5.7

        
        nvgStroke(vg);
    }
};