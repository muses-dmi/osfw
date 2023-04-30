//
//
// TODO: move audio to its own thread
// TODO: put audio data changes, represented by sliders, into slider itself, and so on.
//
#include <math.h>

#include <stdio.h>
#include <iostream>
#include <thread> 
#include <atomic>
#include <memory>

#ifdef NANOVG_GLEW
#	include <GL/glew.h>
#endif
#ifdef __APPLE__
#	define GLFW_INCLUDE_GLCOREARB
#endif
#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>
#include "nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"

#include "pa.h"

#include "osc.hpp"
#include "fm.hpp"
#include "slider.hpp"
#include "scope.hpp"
#include "util.hpp"
#include "globals.hpp"
#include "midi.hpp"
#include "lfspsc_queue.hpp"
#include "button.hpp"

const float sample_rate = 44100;
const int framerate = 30;
const unsigned int num_frames = 256;
const unsigned int num_input_channels = 0;
const unsigned int num_output_channels = 1;

const int width = 640;
const int height = 480;

VSlider gain_slider;
Button frezze_button;
Scope scope;
float gain;
std::shared_ptr<lfspsc_queue<float>> scope_queue;

std::atomic_bool done{false};

//--------------------------------------------------------------------------

// Data that is passed to the audio callback each time it is called.
// We store our pointer(s) to our audio graph here
struct Data {
    FM * fm_;
    Osc * lfo_;
    VSlider * gain_slider_;
    Button * frezze_button_;
    Scope * scope_;
    std::shared_ptr<lfspsc_queue<float>> scope_queue_;
    float * gain_;
};

void audio_callback(const float* in, float* out, long frames, void* data){    
           
    Data* d = static_cast<Data*>(data);

    // Generate frames (samples) for the audio buffer
    for (int i = 0; i < frames; i++) {
        // first increment the LFO, so we can use it to modulate the FM osc
        d->lfo_->inc();

        // now step the FM osc 
        d->fm_->step((1. + d->lfo_->out()) * 1000.);

        float v = d->fm_->out() * gain;
        // finally write the current fm_ sample value to the output channel
        *out++ = v;

        // copy amp to scope queue (it might be full...)
        d->scope_queue_->push(v);
    }
}

// GUI stuff

void errorcb(int error, const char* desc) {
	printf("GLFW error %d: %s\n", error, desc);
}

static void key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	NVG_NOTUSED(scancode);
	NVG_NOTUSED(mods);
	
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
    }

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        scope.freeze();
    }
}

static void mouse_button_callback( GLFWwindow *window, int button, int action, int modS) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if (!gain_slider.start_move(static_cast<int>(xpos), static_cast<int>(ypos))) {
            if (frezze_button.hover(xpos, ypos)) {
                scope.freeze();
            }
        }
    }
    
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        gain_slider.stop_move();
    }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (gain_slider.move(static_cast<int>(ypos))) {
        // calculate any change in gain
        gain = static_cast<double>(map(gain_slider.value(), 0, gain_slider.height(), 0, 1000)) / 1000.;
    }
    else {
        frezze_button.hover(xpos, ypos);
    }
}

void drawframe(GLFWwindow* window, NVGcontext* vg, Data& data) {
    data.gain_slider_->draw(vg);
    data.scope_->draw(vg);
    data.frezze_button_->draw(vg);
}

int main() {
    GLFWwindow* window;
	NVGcontext* vg = NULL;

	if (!glfwInit()) {
		printf("Failed to init GLFW.");
		return -1;
	}

    glfwSetErrorCallback(errorcb);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(width, height, "OSFW", NULL, NULL);
	if (!window) {
		printf("Could not open window\n");
		glfwTerminate();
		return -1;
	}

    glfwSetKeyCallback(window, key);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

	//glfwSetFramebufferSizeCallback(window, resizecb);
	glfwMakeContextCurrent(window);
	//glEnable(GL_POINT_SMOOTH);
	//glEnable(GL_LINE_SMOOTH);

    vg = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_DEBUG);

    if (vg == NULL) {
		printf("Could not init nanovg.\n");
		return -1;
	}

    glfwSwapInterval(0);

    // setup audio
    
    gain_slider = VSlider{70, 320, 30, 120, 60};
    frezze_button = Button{200, 360, 30, 30};

    scope_queue = std::shared_ptr<lfspsc_queue<float>>(new lfspsc_queue<float>{static_cast<size_t>(6*600)});
    scope = Scope{20, 30, 600, 200, sample_rate, num_frames, scope_queue, framerate};

    Osc car{290.f, sample_rate, OSC_TYPE::SINE};
    Osc mod{400.f, sample_rate, OSC_TYPE::SINE};
    Osc lfo{0.7f, sample_rate, OSC_TYPE::TRIANGLE};

    FM fm{mod, car};

    gain = 0.5f;

    Data data{&fm, &lfo, &gain_slider, &frezze_button, &scope, scope_queue, &gain};

 
    // start audio 
    Pa a(audio_callback, num_input_channels, num_output_channels, sample_rate, num_frames, &data);
    a.start(Pa::dontTerminate); // don't block...
    std::cout << "frames: " << a.num_frames() << std::endl;

    // start midi
    std::thread thread_midi (handle_midi);

    // GUI has to run on the main thread
    double current_time = 0.;
    double last_time    = 0.;
    while (!glfwWindowShouldClose(window)) {
        double mx, my, t, dt;
		int winWidth, winHeight;
		int fbWidth, fbHeight;
        float pxRatio;
	
        glfwGetCursorPos(window, &mx, &my);
		glfwGetWindowSize(window, &winWidth, &winHeight);
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
		// Calculate pixel ration for hi-dpi devices.
		pxRatio = (float)fbWidth / (float)winWidth;

        // Update and render
        current_time = glfwGetTime(); 

        

	    if(current_time - last_time >= 1.0 / framerate) {
            //printf("%lf -- %lf\n", current_time - last_time, 1.0 / framerate);
		    last_time = current_time;
            glViewport(0, 0, fbWidth, fbHeight);
            glClearColor(0,0,0,0);
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

            nvgBeginFrame(vg, winWidth, winHeight, pxRatio);
                drawframe(window, vg, data);
            nvgEndFrame(vg);

            glfwSwapBuffers(window);
        }
        glfwPollEvents();
	}

    done = true;

    glfwTerminate(); // glfw
    a.terminate();     // portaudio

    thread_midi.join();

    return 0;
}

