sudo apt install -y git libzmq3-dev libsodium-dev libconfig-dev libasound2-dev build-essential device-tree-compiler cmake libboost-dev libboost-filesystem-dev libconfig++-dev raspberrypi-kernel-headers bc

sudo mkdir /etc/sonomkr

cd ~

mkdir libs -p
cd libs

git clone https://github.com/zeromq/zmqpp.git
cd zmqpp

make
sudo make install

cd ../..

git clone https://github.com/SonoMKR/sonomkr-core.git
cd sonomkr-core

mkdir build
cd build 

cmake -DCMAKE_BUILD_TYPE=Release -DRASPI=true ..
make
sudo make install

cd ../..
