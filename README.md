# Optical Sound Playground

# Building 

Only works on Mac.

Before you can build this, the following need to be setup and installed:

Install [Brew](https://brew.sh/).

The from the comamnd line use Brew to install:

```shell
brew install portaudio
brew install cmake
brew install premake
brew install glfw3
brew install gnu-sed
```

You also need to build and install NanoVG:

```shell
git clone https://github.com/memononen/nanovg
cd nanovg
git submodule add -b master https://github.com/copotron/cmake-nanovg.git
cd cmake-nanovg
mkdir build
cd build
cmake ..
make
sudo make install
```

You can now remove the NanoVG directory:

```shell
cd ..
rm -rf nanovg
```

Now everything is ready, so:

```shell
git clone 
cd osfw
git submodule update --init --recursive
make
```

Run the app with:

```shell
./osfw_app
```