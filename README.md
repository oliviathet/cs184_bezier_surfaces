# cs184_bezier_surfaces
UC Berkeley CS 184 (Computer Graphics) Bezier Surfaces partner project

Displayed Bezier surfaces using uniform and adaptive polygonal tessellation



To compile:
make

To run:

./as3 (.bez/.obj file) (subdivision parameter) (-a) (-o objFilename)



Flags:

object file (.obj) to be converted to a bezier file (.bez): -o filename.obj

adaptive tesselation (default is uniform tesselation): -a

Keys:
+/-: Zooms in/out 

Left/right arrow keys : Rotates object about the y-axis

Up/down arrow keys : Rotates object about the x-axis 

,/. : Rotates object about the z-axis 

Shift + left/right arrow keys : Translates object about x-axis 

Shift + up/down arrow keys : Translates object about y-axis 

Page down/page up keys : Translates object about z-axis 

Alt + up/down arrow keys : Alternative command for translation about z-axis 

w : Toggles between wireframe and filled modes 

s : Toggles between flat and smooth shading modes (only works when not in wireframe mode) 

r : Resets camera to original state 

h : Toggles between filled and hidden-line mode (for wireframe view) 

Space bar : Exits program 
