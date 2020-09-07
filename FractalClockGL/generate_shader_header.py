import sys
import os

def append_shader_to_h(source_path, filename, name):
    file = open(filename, 'r')
    text = file.read().split('\n')
    file.close()

    file = open(source_path, 'a+')
    s = "#define " + name + " \"\\\n"
    for line in text:
        s += line + "\\n\\\n"
    
    s += "\"\n"
    file.write(s)


def clear_shader_h(source_path):
    file = open(source_path, "w+")

print("GENERATING SHADER HEADER FILE")

source_path = os.path.join(sys.argv[1], "InternalShader.h")
clear_shader_h(source_path)
append_shader_to_h(source_path, os.path.join(sys.argv[1], "Resources/Shaders/Vertex.shader"), "VERTEX_SHADER_TEXT")
append_shader_to_h(source_path, os.path.join(sys.argv[1], "Resources/Shaders/Geom.shader"), "GEOM_SHADER_TEXT")
append_shader_to_h(source_path, os.path.join(sys.argv[1], "Resources/Shaders/Frag.shader"), "FRAG_SHADER_TEXT")

print("SHADER FILES UPDATED")