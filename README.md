## Setup:
```
git clone --recurse-submodules https://github.com/provablyprivate/cppexample.git
```

## Install dependencies:
* Arch Linux: `sudo pacman -S poco`
* Ubuntu: `sudo apt-get install libpoco-dev`

## Build
```
cmake . 
make
```

## Simulation dependecies (launcher.py)
* Mininet: http://mininet.org/
* Open vSwitch: http://www.openvswitch.org/, including openvswitch-testcontroller
* xterm: https://invisible-island.net/xterm/
* tcpflow: https://github.com/simsong/tcpflow
