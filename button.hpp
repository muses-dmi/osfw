//
//
// slider

#pragma once

#include "nanovg.h"

class Button {
private:
    int x_;
    int y_;
    int width_;
    int height_;
    bool hover_;
public:
    Button() :
       x_{0}, y_{0}, width_{0}, height_{0}, hover_{false} {
    }

    Button(int x, int y, int width, int height) :
        x_{x}, y_{y}, width_{width}, height_{height}, hover_{false} {

    }

    void draw(NVGcontext* vg) const {
        nvgBeginPath(vg);
        
        nvgRect(vg, x_, y_, width_, height_);
        if (hover_) {
            nvgFillColor(vg, nvgRGBA(100,192,0,255));
        }
        else {
            nvgFillColor(vg, nvgRGBA(100,192,0,100));
        }
        nvgFill(vg);
    }

    bool hover(int x, int y) {
        if (x >= x_ &&  x <= x_ + width_ && y >= y_ && y < y_ + height_) { 
            hover_ = true;
            return true;
        }

        hover_ = false;
        return false;
    }

    int width() const {
        return width_;
    }

    int height() const {
        return height_;
    }
};