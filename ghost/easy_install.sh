#!/usr/bin/env bash
# ------------ easy_install.sh ------------
#   • Compiles GHost++ and assembles the runtime folder `bot/`
#   • Assumes all system packages were installed by the Dockerfile.
#   • Uses limited parallel jobs to avoid OOM in low‑memory Docker builds.
set -e

JOBS=${MAKE_JOBS:-2}       # default 2 parallel jobs; override with MAKE_JOBS

# 1) bncsutil
cd src/bncsutil/src/bncsutil
make -j${JOBS}
sudo make install

# 2) StormLib
cd ../../../StormLib/stormlib
make -j${JOBS}
sudo make install

# 3) Main GHost++ build
cd ../../
make -j${JOBS}

# 4) Assemble runtime directory
cd ../
mkdir -p bot
cp src/ghost++           bot/
cp -R config/*           bot/
cp -R plugins            bot/

echo "[easy_install] Build complete.  Run the bot with:  cd bot && ./ghost++"
