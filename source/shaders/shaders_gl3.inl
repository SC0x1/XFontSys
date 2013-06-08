// Copyright © 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose: the glsl shades
//

// Vertex shader
const char vertexShader[] = 
    "\
    #version 150 core\n \
    #extension GL_ARB_explicit_attrib_location : enable\n \
    layout(location = 0) in vec4 Position;\n \
    layout(location = 5) in vec4 Texcoord0;\n \
    layout(location = 2) in vec4 Color;\n \
    uniform vec2 uScale;\n \
    out block{\n \
        vec4 Texcoord0;\n \
        vec4 Color;\n \
    } Out;\n \
    void main()\n \
    {\n \
        Out.Texcoord0 = Texcoord0;\n \
        Out.Color = Color / 255;\n \
        gl_Position = vec4(uScale, uScale) * Position;\n \
    }\n";

// Geometry shader
const char geometryShader[] = 
    "\
    #version 150 core\n \
    layout (points) in;\n \
    layout (triangle_strip, max_vertices = 4) out;\n \
    in block{\n \
        vec4 Texcoord0;\n \
        vec4 Color;\n \
    } In[];\n \
    out block{\n \
        vec2 Texcoord0;\n \
        vec4 Color;\n \
    } Out;\n \
    #define IN_POS (gl_in[0].gl_Position)\n \
    void main()\n \
    {\n \
        gl_Position = vec4(-1 + IN_POS.x, 1 - IN_POS.y, 0, 1);\n \
        Out.Texcoord0 = In[0].Texcoord0.st;\n \
        Out.Color = In[0].Color;\n \
        EmitVertex();\n \
        gl_Position = vec4(-1 + IN_POS.z, 1 - IN_POS.y, 0, 1);\n \
        Out.Texcoord0.s = In[0].Texcoord0.s + In[0].Texcoord0.p;\n \
        Out.Texcoord0.t = In[0].Texcoord0.t;\n \
        Out.Color = In[0].Color;\n \
        EmitVertex();\n \
        gl_Position = vec4(-1 + IN_POS.x, 1 - IN_POS.w, 0, 1);\n \
        Out.Texcoord0.s = In[0].Texcoord0.s;\n \
        Out.Texcoord0.t = In[0].Texcoord0.t + In[0].Texcoord0.q;\n \
        Out.Color = In[0].Color;\n \
        EmitVertex();\n \
        gl_Position = vec4(-1 + IN_POS.z, 1 - IN_POS.w, 0, 1);\n \
        Out.Texcoord0.s = In[0].Texcoord0.s + In[0].Texcoord0.p;\n \
        Out.Texcoord0.t = In[0].Texcoord0.t + In[0].Texcoord0.q;\n \
        Out.Color = In[0].Color;\n \
        EmitVertex();\n \
        EndPrimitive();\n \
    }\n";

// Fragment shader
const char fragmentShader[] = 
    "\
    #version 150 core\n \
    uniform sampler2D uTexUnit0;\n \
    in block{\n \
    vec2 Texcoord0;\n \
    vec4 Color;\n \
    } In;\n \
    out vec4 FragColor;\n \
    void main()\n \
    {\n \
        FragColor = vec4(1, 1, 1, texture(uTexUnit0, In.Texcoord0).r ) * In.Color;\n \
    }\n";