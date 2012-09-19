
//---------------------------------------------------------------------------*/
// GLSL - Shaders. For the text render shader program  (without the matrix transform) 
//---------------------------------------------------------------------------*/

// TODO : передавать цвет через аттрибуты (4 ubytes)

// the vertex shader simply passes through data
const char VertexShader[] = "#version 150 core\n \
							#extension GL_ARB_explicit_attrib_location : enable\n \
							layout(location = 0) in vec4 a_Vertex;\n \
							layout(location = 1) in vec4 a_TexCoord0;\n \
							layout(location = 2) in vec4 a_Color;\n \
							out vec4 vs_TexCoord0;\n \
							out vec4 vs_Color;\n \
							void main( void ) {\n \
								vs_TexCoord0 = a_TexCoord0;\n \
								vs_Color = (a_Color / 255);\n \
								gl_Position = a_Vertex;\n \
							}\n";

// the geometry shader creates the billboard quads
const char GeometryShader[] = "#version 150 core\n \
							layout (points) in;\n \
							layout (triangle_strip, max_vertices = 4) out;\n \
							in vec4 vs_TexCoord0[];\n \
							in vec4 vs_Color[];\n \
							out vec2 gs_TexCoord0;\n \
							out vec4 gs_Color;\n \
							uniform vec2 u_Scale;\n \
							void main() {\n \
								vec4 qv = gl_in[0].gl_Position;\n \
								vec4 tex = vs_TexCoord0 [0];\n \
								gl_Position = vec4(-1 + (qv.x * u_Scale.x), 1 - (qv.y * u_Scale.y), 0, 1);\n \
								gs_TexCoord0.xy = tex.xy;\n \
								gs_Color = vs_Color[0];\n \
								EmitVertex();\n \
								gl_Position = vec4(-1 + (qv.z * u_Scale.x), 1 - (qv.y * u_Scale.y), 0, 1);\n \
								gs_TexCoord0.x = tex.x + tex.z;\n \
								gs_TexCoord0.y = tex.y;\n \
								gs_Color = vs_Color[0];\n \
								EmitVertex();\n \
								gl_Position = vec4(-1 + (qv.x * u_Scale.x), 1 - (qv.w * u_Scale.y), 0, 1);\n \
								gs_TexCoord0.x = tex.x;\n \
								gs_TexCoord0.y = tex.y + tex.w;\n \
								gs_Color = vs_Color[0];\n \
								EmitVertex();\n \
								gl_Position = vec4(-1 + (qv.z * u_Scale.x), 1 - (qv.w * u_Scale.y), 0, 1);\n \
								gs_TexCoord0.x = tex.x + tex.z;\n \
								gs_TexCoord0.y = tex.y + tex.w;\n \
								gs_Color = vs_Color[0];\n \
								EmitVertex();\n \
								EndPrimitive();\n \
							}\n";

// the fragment shader
const char FragmentShader[] = "#version 150 core\n \
							uniform sampler2D textureUnit0;\n \
							in vec2 gs_TexCoord0;\n \
							in vec4 gs_Color;\n \
							out vec4 FragColor;\n \
							void main( void ) {\n \
								FragColor = vec4(1, 1, 1, texture(textureUnit0, gs_TexCoord0).r ) * gs_Color;\n \
							}\n";
