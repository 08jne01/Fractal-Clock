# Fractal-Clock

It simply takes a regular analogue clock then on the end's of each hands it plops a new clock but 
slightly smaller. This is one iteration. You can repeat this step but many times and you build some interesting fractals
depending on what the local time is.

The idea is not my own however the implemntation is. The original idea can be found on the awesome youtube channel [CodeParade](https://youtu.be/4SH_-YhN15A).

The implementation makes use of graphics tricks to improve performance and reduce CPU load. For example instead of the CPU
loading all the vertices for each clock it instead loads a single vertex which contains the clock origin rotation and size.
From this data the final clock geometry can be created in a geometry shader. Which also creates the thick lines (which the width
is also passed in the vertices). 

The process is also heavily multithreaded on both the clock update and the CPU side of the draw call. There are still improvements
to be made but I am pretty happy with the result.

Lastly custom shaders can be loaded in. This can be done using the following command:
```FractalClock.exe --shaders```

The program will try to look for the following shader:
```
Resources/shaders/Vertex.shader
Resources/shaders/Geom.shader
Resources/shaders/Frag.shader
```

Examples can be found in this repository. If you would like to customise the colours of the clocks then take a look at the Vertex.shader specifically and the getVertexColor()
function.

## Controls
| Key              | Function                               |
|------------------|----------------------------------------|
| Left/Right Arrow | Decrease/Increase Size of Clock        |
| Up/Down Arrow    | Increase/Decrease number of iterations |
| Page Up/Down     | Increase/Decrease Line Width of Clock  |
