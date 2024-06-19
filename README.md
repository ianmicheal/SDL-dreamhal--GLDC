
# DREAMCAST SDL-DREAMHAL Version 1.0
**Based on SDL 1.2.13**

## Overview
SDL-DREAMHAL is a customized version of SDL (Simple DirectMedia Layer) tailored for the Dreamcast. It leverages GLDC for rendering, providing significant performance enhancements for 2D blitting and other operations. This version includes updates for fixed headers and multiple defines.

## Features
- **Dreamcast Hardware + OpenGL**: Optimized for SDL FAST BLIT SPEED.
- **2D Blit Speed Benchmark**: Significant improvements in both software and hardware modes.
- **Integration with GLDC**: Utilizes Kazade's GLDC library for enhanced OpenGL compatibility and performance.

![Screenshot of SDL-DREAMHAL](https://github.com/ianmicheal/SDL-dreamhal--GLDC/blob/main/benchmark.png)

## Dreamcast Integration
- **Correct OpenGL integration.**: 
- **Textured video driver for virtual resolutions.**: 
- **Direct framebuffer video driver using store queues.**:
- **Mouse emulation using analog pad**: 
- **Mouse emulation using analog pad**: 
- **Mouse emulation using analog pad**:
- **Mapped keys as pad buttons.**:
- **Two texture internal color mode supported: RGB5551 and RGB4444.**:
- **Fast threaded audio driver.**:

  ## SDL
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



## Additional Resources

- **PORTED TO SDL + OpenGL Dreamhal + GLDC Demos**:
- **SDL[DREAMHAL]+[GLDC]KOS2.0 TWINKLE STAR DEMO HARDWARE**:
  - [Demo 1](https://youtu.be/aTKOW4GrLsQ)
  -  ** SDL[DREAMHAL]+[GLDC]KOS2.0 BLENDING GLASS CUBE HARDWARE**:
  - [Demo 2](https://youtu.be/GM4JnKxp1ZQ)
  - ** SDL[DREAMHAL]+[GLDC]KOS2.0 SPINNING TEXTURES CUBE 60FPS
  - [Demo 3](https://youtu.be/G0KFthogrPg)
  - **SDLQUAKE DREAMHAL**:
  - [Demo port quake 1].(https://youtu.be/QiEo3FNNEZc)
  - **OLD VERSION CANT BE COMPILED OR USED ON KOS2.0**
   - **Old Version**: [SDL by Chui](http://chui.dcemu.co.uk/sdl.html)

---

For a complete list of features and detailed documentation, please visit the [GLDC repository](https://gitlab.com/simulant/GLdc).

## Credits
- **BERO**: Original work  for SDL on Dreamcast.
- **Chui**: Original work and improvements for SDL on Dreamcast.
- **Kazade**: GLDC library and enhancements.
- **MRNEO240**:GLDC Optimizing and Support.
GNU Lesser General Public License, version 2.1
https://www.gnu.org/licenses/old-licenses/lgpl-2.1.en.html



