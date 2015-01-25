sudo apt-get -qq update
sudo apt-get install libpcap-dev libssl-dev cmake
git clone git@github.com:mfontanini/libtins.git
cd libtins
mkdir build
cd build
cmake ../
make
sudo make install
