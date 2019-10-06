# zweidee

lean 2d framebuffer for game prototyping

* A framebuffer provides a SetPixel method.
  The framebuffer is applied as a texture on an OpenGL Quad of screen (window) size.
* A DrawLib adds rudimentary functions to draw, i.e.
  Bresenham,
  Bresenhem_Circle
* A Game class declares virtual function for init, doit, left,right and fire.

resolution and ratio are controlled by
* framebuffer size
* screen resolution

Examples included:

| game | optimized for | framebuffer | res.screen |
|---|---|---|---|
| comanche |   |   |
| frogger | IOS  | 200 x 320 | 800 x 600 |
| galaga | | 64 x 64  | e.g. 512 x 512 |
| pitstop |   |   |

