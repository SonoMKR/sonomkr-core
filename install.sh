sudo apt install -y git libzmq3-dev libsodium-dev libconfig-dev libasound2-dev build-essential device-tree-compiler cmake libboost-dev libboost-filesystem-dev libconfig++-dev raspberrypi-kernel-headers bc

sudo mkdir /etc/sonomkr

mkdir libs -p
cd libs

git clone https://github.com/zeromq/zmqpp.git
cd zmqpp

git checkout -b 4.2.0

make
sudo make install

cd ../..

mkdir build
cd build 

cmake -DCMAKE_BUILD_TYPE=Release -DRASPI=true ..
make
sudo make install

cd ../..
