#!/bin/bash

# Skip system dependency installation – handled by Dockerfile
# sudo apt-get install -qq libboost-all-dev build-essential gcc g++ libmysql++-dev libbz2-dev libgmp3-dev

cd src/bncsutil/src/bncsutil
make
sudo make install

cd ../../../StormLib/stormlib
make
sudo make install

cd ../../
make

cd ../
mkdir -p bot
cp src/ghost++ bot/
cp -R config/* bot/
cp -R plugins bot/
