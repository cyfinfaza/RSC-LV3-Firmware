# RSC-LV3-Firmware  
This is a monorepo containing the individual firmware projects for each of low-voltage system modules on the 2026 Rutgers Solar Car, as well as shared libraries (just the CAN subsystem right now).

## Build outputs
Each project's `build/` folder is gitignored. Instead, every build copies the `.elf` and `.map` into `<project>/bin/<build type>/` (e.g. `Dashboard/bin/Debug/Dashboard.elf`), and those get committed so firmware can be downloaded and flashed without building.

When creating a new project, copy the block marked `RSC: copy build outputs to committed bin/ folder` from the bottom of an existing project's `CMakeLists.txt` into the new one, and add `/build/` to its `.gitignore`.
