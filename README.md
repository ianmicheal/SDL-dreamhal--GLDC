
# SDL-DREAMHAL Version 1.0
**Based on SDL 1.2.13**



## Overview
SDL-DREAMHAL is a customized version of SDL (Simple DirectMedia Layer) tailored for the Dreamcast. It leverages GLDC for rendering, providing significant performance enhancements for 2D blitting and other operations. This version includes updates for fixed headers and multiple defines.

## Features
- **Dreamcast Hardware + OpenGL**: Optimized for SDL FAST BLIT SPEED.
- **2D Blit Speed Benchmark**: Significant improvements in both software and hardware modes.
- **Integration with GLDC**: Utilizes Kazade's GLDC library for enhanced OpenGL compatibility and performance.

![Screenshot of SDL-DREAMHAL](https://github.com/ianmicheal/SDL-dreamhal--GLDC/blob/main/benchmark.png)

## Dreamcast Integration
- **Arch**: Shutting down kernel.
- **Maple**: Final stats -- device count = 2, vbl_cntr = 56270, dma_cntr = 56266.
- **Video Mode**: Set to 640x480 NTSC.

## Simple DirectMedia Layer (SDL)
SDL is a cross-platform multimedia library designed to provide low-level access to audio, keyboard, mouse, joystick, 3D hardware via OpenGL, and 2D video framebuffer.

### New Features
- **Upgraded Blit Speed**: Utilizing moops dreamhal SH4 ASM memcpy and memset.
- **GLDC Integration**: Improved compatibility and performance with OpenGL 1.2.

### Video Drivers for Dreamcast
- **SDL_DC_DMA_VIDEO** (default): Fastest video driver using double buffer.
- **SDL_DC_TEXTURED_VIDEO**: Allows virtual resolutions and scaling using hardware texture.
- **SDL_DC_DIRECT_VIDEO**: Direct buffer video driver, potentially faster than DMA without double buffer.

### Configuration Options
- **SDL_DC_VerticalWait(SDL_bool value)**: Enable/disable wait for vertical retrace before blitting to PVR hardware.
- **SDL_DC_ShowAskHz(SDL_bool value)**: Enable/disable ask for 50/60Hz video (only for PAL Dreamcasts).
- **SDL_DC_Default60Hz(SDL_bool value)**: Set default display to 60Hz (only for PAL Dreamcasts).
- **SDL_DC_MapKey(int joy, SDL_DC_button button, SDLKey key)**: Map Dreamcast buttons to SDL keys.

## Credits
- **Chui**: Original work and improvements for SDL on Dreamcast.
- **Kazade**: GLDC library and enhancements.

## Additional Resources
- **Old Version**: [SDL by Chui](http://chui.dcemu.co.uk/sdl.html)
- **OpenGL Dreamhal + GLDC Demos**:
  - [Demo 1](https://streamable.com/55982r)
  - [Demo 2](https://streamable.com/kvk4ez)
  - [Demo 3](https://streamable.com/5k8lsk)

---

For a complete list of features and detailed documentation, please visit the [GLDC repository](https://gitlab.com/simulant/GLdc).


## Badges

Add badges from somewhere like: [shields.io](https://shields.io/)

[![MIT License](https://img.shields.io/badge/License-MIT-green.svg)](https://choosealicense.com/licenses/mit/)
[![GPLv3 License](https://img.shields.io/badge/License-GPL%20v3-yellow.svg)](https://opensource.org/licenses/)
[![AGPL License](https://img.shields.io/badge/license-AGPL-blue.svg)](http://www.gnu.org/licenses/agpl-3.0)


![Logo](https://dev-to-uploads.s3.amazonaws.com/uploads/articles/th5xamgrr6se0x5ro4g6.png)


## Run Locally

Clone the project

```bash
  git clone https://link-to-project
```

Go to the project directory

```bash
  cd my-project
```

Install dependencies

```bash
  npm install
```

Start the server

```bash
  npm run start
```

