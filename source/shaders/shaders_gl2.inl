// Copyright © 2013 Vitaly Lyaschenko (scxv86@gmail.com)
// Purpose: the glsl shades
//

//
// Vertex shader
const char vertexShader[] =
    "\
    attribute vec4 aVT;\n \
    varying vec2 vTexcoord0;\n \
    varying vec4 vColor;\n \
    uniform vec2 uScale;\n \
    uniform vec4 uColor;\n \
    void main()\n \
    {\n \
        vTexcoord0 = aVT.zw;\n \
        vColor = (uColor / 255.0);\n \
        gl_Position = vec4(-1.0 + (aVT.x * uScale.x), 1.0 - (aVT.y * uScale.y), 0.0, 1.0);\n \
    }\n";

//
// Fragment shader
const char fragmentShader[] =
    "\
    varying vec2 vTexcoord0;\n \
    varying vec4 vColor;\n \
    uniform sampler2D uTexUnit0;\n \
    void main()\n \
    {\n \
        gl_FragColor = vec4(1.0, 1.0, 1.0, texture2D(uTexUnit0, vTexcoord0).r ) * vColor;\n \
    }\n";
