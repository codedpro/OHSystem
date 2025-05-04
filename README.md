# GHost++ (OHSystem) — 2025 Version by Codedpro

This repository provides a Dockerized environment for building and running the classic [OHSystem/GHost++](https://github.com/ohsystem/ghostplusplus) bot, with full **Docker support** for compiling and running on **modern systems** including **macOS (Intel & ARM), Linux, and Docker Desktop environments**.

## ✨ Key Features

- Dockerfile with full support for legacy dependencies (GCC 4.8, Boost 1.54, Python 2.7, MySQL++)
- Updated `easy_install.sh` script for headless builds
- Works on modern platforms including Apple Silicon (M1/M2)
- One-liner export of the compiled bot and runtime files

---

## 📆 How to Build the Image

```bash
git clone https://github.com/codedpro/OHSystem.git
cd OHSystem/ghost
docker buildx build --platform linux/amd64 -t ghost-legacy --load .
```

This compiles GHost++ with all dependencies inside a clean Docker container.

---

## 📤 Export the Compiled Bot

> You must first start a container from the built image to extract its contents:

```bash
docker create --name ghost-temp ghost-legacy
```

Then run the following to copy the compiled files into your current directory:

```bash
docker cp ghost-temp:/ghost/bot .
docker rm ghost-temp
```

You will get a folder like:

```
./bot/
├── ghost++         # Compiled bot binary
├── default.cfg     # Main bot config
├── mysql.cfg       # Database config (optional)
├── plugins/        # Optional plugin support
├── languages/      # Language localization files
├── *.txt files     # insult.txt, rules.txt, etc.
```

---

## 📅 Runtime Notes

- You **must** include `default.cfg` alongside the binary.
- The binary expects maps to be present in a `maps/` folder if hosting is enabled.
- Plugins and other configs will be loaded automatically if present in the same directory.

---

## 📊 Credits

Based on the legendary [OHSystem/GHost++](https://github.com/ohsystem/ghostplusplus)

Docker modernization and build tooling by [codedpro](https://github.com/codedpro)

