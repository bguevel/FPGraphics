Last Lap Studios
Andrew Bernklau, Edward Gibson - ewgibson@mines.edu, Ben Guevel
FP / The Grey Havens

Description:
This program is a racing game where two players, an AI and the user, race around a track.
The track is set in a forest area w/ a gravel speedway. The track itself is not entirely flat.
Height mapping is used to displace the track to various heights adding a decent amount of bumps.
The AI car follows a series of Bézier curves generated around the track.
It also changes its direction tangential to it's position on the curves.
The player moves the cart with 'wasd' keys and after reaching a certain speed, speed lines are generated behind the player car.
The program allows for the change of many POVs: a fixed POV, an arc ball cam, and a first person view.
The cars themselves are animated as they drive, the wheels rotate along their 'axles' or when turning they rotate toward that direction.

"Seek the arc where colors play, turn your wheels the other way"

Controls:
        W A S D - forwards, backwards, and side to side for the character in the world
        Mouse Drag - Pan camera side to side and up and down
        Mouse Drag up and down + shift - Zoom camera in and out
        1 - Fixed POV
        2 - Arc ball cam
        3 - First person view
        Q / ESC - quit

Compilation and running
Command line:
cmake -S . -B build
cmake --build build
./build/FP

Through CLion:
set target to FP
build and run

No known bugs that of which are known. Although, the user can drive anywhere on and off the track and collisions are not implemented.

Andrew - Added Bezier Curve w/ arc length parameterization for the AI cart and implemented the speed lines with v/g/f shaders.
Ben - Set up the world by incorporating lighting, skybox, and height mapping.
Edward - Created cart objects w/ headlights, implemented textures and materials, added objects to the scene, and hid an Easter egg

EASTER EGG:
In order to access the easter egg, you have to go around the track backwards.

Q1. Several days
Q2. 10
Q3. 10