Zweidee
=======
Use 3d rendering
- OpenGL (Windows)
- Metal (IOS)
for 2D a frambuffer.


How to add a game:
i) make a copy of a game directory, e.g. galaga
ii) rename all occurrences of "galaga" e.g. to "glenz"
    rename the class to e.g. CGlenz,
    all virtual functions have to be implemented!
iii) in engine.h add the game


===================
--- deprecated: ---
1) Enable game type in proj/proj.h:

    // Generic Variables
//    galaga::CGalaga   m_game;
//    pitstop::CPitstop   m_game;
//    comanche::CComanche m_game;
    frogger1tap::CFrogger m_game;

2) Adapt resolution