TARGET = osfw_app
SRC = main.cpp midi.cpp paw/pa.cpp

INCLUDE = -I./paw -I/opt/homebrew/include/ -I./3rd_party/inc -I./3rd_party/libremidi/include
LIBS = -lportaudio -lnanovg -lglfw

# on mac/linux: libs are usually installed in /usr/lib which is likely in PATH
LIBS += -L/opt/homebrew/lib/ -L./3rd_party/lib

CXX = clang++
FLAGS = -DLIBREMIDI_HEADER_ONLY=1 -DLIBREMIDI_COREAUDIO=1 -std=c++20 -Wno-deprecated -framework OpenGL -framework CoreMIDI -framework CoreAudio -framework CoreFoundation

HEADERS = osc.hpp fm.hpp slider.hpp util.hpp scope.hpp ./paw/pa.h lfspsc_queue.hpp button.hpp

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	$(CXX) $(INCLUDE) $(FLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)

run:
	@./$(TARGET)
