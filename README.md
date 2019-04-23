### Clone
```
git clone --recurse-submodules https://github.com/provablyprivate/cppexample.git
```

### Install build dependencies
* Arch Linux: `sudo pacman -S poco`
* Ubuntu: `sudo apt-get install libpoco-dev`

### Build
```
cmake . 
make
```
# Running the simulation (launcher.py)

### Dependencies
* Mininet: http://mininet.org/
* Open vSwitch: http://www.openvswitch.org/, including openvswitch-testcontroller
* xterm: https://invisible-island.net/xterm/
* tcpflow: https://github.com/simsong/tcpflow

### Arch Linux installation
* `sudo pacman -S core/bison`
* `sudo pacman -S core/flex`
* `yay -S aur/mininet-git`
* `sudo pacman -S community/openvswitch`
* `sudo pacman -S extra/xterm`
* `sudo pacman -S community/tcpflow`

Might require a reboot.

### Run:
sudo python launcher.py

