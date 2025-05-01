# GHost++ (OHSystem) — Dockerized Fork by codedpro

This repository is a modernized fork of the original\*\* **[OHSystem/GHost++](https://github.com/ohsystem/ghostplusplus), enhanced with full** \***\*Docker support** to make compiling and running the bot easier on** \*\***modern systems** including** \*\* **macOS (Intel & ARM), Linux, and Docker Desktop environments** .

## ✨ Key Features

- Dockerfile with full support for legacy dependencies (GCC 4.8, Boost 1.54, Python 2.7, MySQL++)
- Updated\*\* \*\*`easy_install.sh` script for headless builds
- Works on modern platforms including Apple Silicon (M1/M2)
- Single-command export of compiled binary and necessary runtime files

---

## 📆 How to Build the Image

```bash
git clone https://github.com/codedpro/OHSystem.git
cd OHSystem
docker buildx build --platform linux/amd64 -t ghost-legacy --load .
```

This compiles GHost++ with all dependencies inside a clean Docker container.

---

## 🚀 Run and Test Inside Docker

```bash
docker run -it --rm ghost-legacy
cd bot
./ghost++
```

You should see:

```
[GHOST] using config file [default.cfg]
[GHOST] starting GHost++ v...
```

---

## 📤 Export the Compiled Bot

Run this on your host machine to extract everything required to run the bot outside Docker:

```bash
docker create --name ghost-temp ghost-legacy

docker cp ghost-temp:/ghost/ghost++ ./ghost-ready/ghost++
docker cp ghost-temp:/ghost/config ./ghost-ready/config
docker cp ghost-temp:/ghost/plugins ./ghost-ready/plugins
docker cp ghost-temp:/ghost/mapcfgs ./ghost-ready/mapcfgs

docker rm ghost-temp
```

You will get a folder like:

```
ghost-ready/
├── ghost++         # Compiled bot binary
├── config/         # Main .cfg files like default.cfg, map.cfg
├── plugins/        # Optional plugin support
├── mapcfgs/        # Map-specific configurations
```

---

## 📅 Runtime Notes

- You** \*\***must** include** **`default.cfg` in the** \*\*`config/` folder.
- The binary expects maps to be present in a\*\* \*\*`maps/` folder if hosting is enabled.
- The\*\* **`plugins/` and** \*\*`mapcfgs/` folders are optional, depending on your config.

---

## 📊 Credits

Based on the legendary\*\* \*\*[OHSystem/GHost++](https://github.com/ohsystem/ghostplusplus)

Docker modernization and build tooling by\*\* \*\*[codedpro](https://github.com/codedpro)
