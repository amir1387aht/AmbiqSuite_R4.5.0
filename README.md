# miband8_reverse_engineering

> Custom firmware and build instructions for the Mi Band 8 (community project).

---

## 🎯 Summary
This repository contains a custom firmware build for the Mi Band 8 (Apollo4BL-based). Watch the video (here)[https://youtu.be/4HnEjwFwrM8?si=52hVDTkAnvSoc9ze]

---

## 📋 Table of Contents
- [Prerequisites](#prerequisites)  
- [Build & Flash](#build--flash)  
- [Pinout / Hardware reference](#pinout--hardware-reference)  
- [Troubleshooting](#troubleshooting)  
- [Contributing](#contributing)  
- [License](#license)

---

## ⚙️ Prerequisites
- A development environment with `make` and a GCC toolchain for Apollo4 (or the toolchain you use for this project).  
- Windows / Linux shell depending on `flash.bat` usage (Windows recommended for `.bat`; use equivalent script on Linux).  
- USB/UART adapter or the specific flasher required by your board.  
- Basic familiarity with the command line.

---

## 🛠 Build & Flash (copy-paste)

1. Open a terminal in the repo root (or navigate there first).
2. Change to the GCC project folder:
```bash
cd boards/apollo4l_blue_evb/projects/smartwatchs/mb8_smartwatch/gcc
````

3. Build:

```bash
make
```

4. Flash (Windows):

```bash
./flash.bat
```

> If you are on Linux/macOS you likely need to run an equivalent script (e.g. `./flash.sh`) or use the appropriate uploader tool — adapt as needed.

---

## 🔌 Pinout / Hardware reference

For pinout and hardware wiring reference, see the excellent community resource:

* ATCmiBand8fw pinout repo:
  [https://github.com/atc1441/ATCmiBand8fw](https://github.com/atc1441/ATCmiBand8fw)

Use that repo to confirm UART pins, power, and boot-mode entry for your specific Mi Band 8 variant.

---

## 🧯 Troubleshooting

**Build fails**

* Ensure the correct GCC toolchain for Apollo4 is installed and in your `PATH`.
* Look at the `Makefile` for expected environment variables (e.g., `CC`, `CFLAGS`, `SDK_PATH`).

**Flash fails**

* Confirm your USB/UART adapter is on the correct COM port (/dev/tty*).
* Verify wiring (TX↔RX swap, GND connected).
* Ensure the device is in bootloader mode (some devices require button press or pin shorting).
* If `flash.bat` uses windows-only tools, run from Command Prompt or PowerShell with admin rights.

**Device bricked / not booting**

* Re-check pinout and voltage levels.
* Use a serial console to watch boot logs if available.
* Look for bootloader recovery procedure in the ATC repo or related threads.

---

## 🤝 Contributing

We welcome improvements:

1. Fork the repo.
2. Create a branch for your feature/fix.
3. Open a Pull Request with clear testing steps and device model tested.

Please include:

* Board / Mi Band 8 variant used
* Toolchain version
* Exact commands you ran and their output (or logs)

---

## ⚖️ License

Apache License 2.0
