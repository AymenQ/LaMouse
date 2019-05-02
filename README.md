# COMP2215 LaFortuna project - LaMouse

This project aims to allow the LaFortuna to be used as a USB mouse.

## Build instructions
Clone the repository, and ensure that submodules are initialised with:
`git submodule update --init --recursive`
Then to build the project, run `make`. To upload the application, run `make upload` with the LaFortuna plugged in.

## Usage
Once the application is uploaded, you can plug the LaFortuna any Laptop/PC USB port and use it as a mouse - the left/up/down/right buttons move in their respective directions, and the centre button functions as a mouse click.

## Attribution
* FortunaOS -- Klaus-Peter Zauner, adapted by Steve Gunn
* LUFA (USB framework) -- Dean Camera (https://github.com/abcminiuser/lufa)
* Descriptors.h/c and understanding taken from LUFA demo projects
* Build system based off fortuna-pass (and LUFA) -- Cyanogenoid (https://github.com/Cyanogenoid/fortuna-pass)
