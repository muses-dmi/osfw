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
    int wt2_;

    int count_;
    bool freeze_;
    bool border_;
public:
    Scope() :
       x_{0}, y_{0}, 
       width_{0}, height_{0}, 
       sample_rate_{0.},
       buffer_size_{0}, buffer_{nullptr},
       current_offset_{0}, data_{nullptr},
       framerate_{0},
       wt2_{0},
       count_{0},
       freeze_{false},
       border_{false} {
    }

    Scope(
        int x, int y, 
        int width, int height, 
        float sample_rate,
        unsigned int num_frames, 
        std::shared_ptr<lfspsc_queue<float>> buffer,
        int framerate,
        bool border = false) :
        x_{x}, y_{y}, 
        width_{width}, height_{height}, 
        sample_rate_{sample_rate},
        buffer_size_{num_frames}, buffer_{buffer},
        current_offset_{0}, data_{new int[height]},
        framerate_{framerate},
        wt2_{width_ / 2},
        count_{0},
        freeze_{false},
        border_{border} {
            for (int i = 0; i < height; i++) {
                data_[i] = 0;
            }
    }

    ~Scope() {
    }

    void freeze() {
        freeze_ = !freeze_;
    }

    void draw(NVGcontext* vg) {
        nvgBeginPath(vg);
        
        if (border_) {
            // draw rectangle to give the scope shape 
            nvgRect(vg, x_, y_, width_, height_);
        }
        
        nvgStrokeColor(vg, nvgRGBA(255,255,255,255));

        if (!freeze_) {
            for (int i = 0; i < height_ - buffer_size_; i++) {
                // copy previous data to left by buffer size
                data_[i] = data_[i+buffer_size_];
            }

            for (int i = height_ - buffer_size_; i < height_; i++) {
                
                float amp = 0.;
                buffer_->pop(amp);

                // first scale 
                amp = amp * wt2_;
                data_[i] = amp;
            }
        }
        count_ = count_ + 1;
        if (count_ == 6) {
            count_ = 0;
        }

        for (int i = 0; i < height_; i++) {
            nvgMoveTo(vg, x_ + wt2_, y_+ i);
            nvgLineTo(vg, (x_ + wt2_) + data_[i] * -1, y_+ i);
        }
        
        nvgStroke(vg);
    }
};