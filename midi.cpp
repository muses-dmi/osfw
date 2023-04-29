//
//
// midi processing


#include <libremidi/libremidi.hpp>

#include "globals.hpp"
#include "slider.hpp"
#include "midi.hpp"
#include "util.hpp"

void handle_midi() {
    libremidi::midi_in midi;
    for(int i = 0, N = midi.get_port_count(); i < N; i++) {
        // Information on port number i
        std::string name = midi.get_port_name(i);

        std::cout << "port: " << name << std::endl;
    }
    midi.open_port(1); 

    while(!done) {
        libremidi::message next_message;
        if(midi.get_message(next_message)) {
            // next_message holds a valid message.
            if (next_message.get_message_type() == libremidi::message_type::CONTROL_CHANGE &&
                next_message.get_channel() == 1 &&
                next_message.bytes[1] == 20) {
                std::cout << "value: " << static_cast<int>(next_message.bytes[2]) << std::endl;
                double value = static_cast<double>(next_message.bytes[2]);
                gain = value / 127.f;
                // update gain slider
                
                gain_slider.set(static_cast<double>(map(value, 0, 127, 0, gain_slider.height())));
            }
        }
    }
}
