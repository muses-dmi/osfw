//
//
// slider

#pragma once

#include "nanovg.h"

class VSlider {
private:
    int x_;
    int y_;
    int width_;
    int height_;
    int offset_;
    int moving_ypos_;
    bool sliding_;
    // TODO: add colour
public:
    VSlider() :
       x_{0}, y_{0}, width_{0}, height_{0}, moving_ypos_{0}, sliding_{false} {
    }

    VSlider(int x, int y, int width, int height, int offset) :
        x_{x}, y_{y}, width_{width}, height_{height}, offset_{offset}, sliding_{false} {

    }

    void draw(NVGcontext* vg) const {
        nvgBeginPath(vg);
        nvgRect(vg, x_, y_, width_, height_);
        nvgStrokeColor(vg, nvgRGBA(255,192,0,255));
        nvgStroke(vg);

        nvgBeginPath(vg);
        nvgRect(vg, x_, y_ + (height_-offset_), width_, offset_);
        nvgFillColor(vg, nvgRGBA(255,192,0,255));
        nvgFill(vg);
    }

    bool start_move(int x, int y) {
        if (x >= x_ &&  x <= x_ + width_ && y >= y_ && y < y_ + height_) { 
            sliding_ = true;
            moving_ypos_ = y;

            return true;
        }

        return false;
    }

    bool stop_move() {
        if (sliding_) {
            sliding_ = false;
            return true;
        }

        return false;
    }

    bool move(int y) {
        if (sliding_) {
            offset_ += moving_ypos_ - y;
            if (offset_ > height_) {
                offset_ = height_;
            }
            else if (offset_ < 0) {
                offset_ = 0;
            }
            moving_ypos_ = y;
            return true;
        }
        return false;
    }

    void set(int v) {
        offset_ = v;
        //TODO: validate in range
    }

    int value() const {
        return offset_;
    }

    int width() const {
        return width_;
    }

    int height() const {
        return height_;
    }
};