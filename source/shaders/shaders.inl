
//---------------------------------------------------------------------------*/
// GLSL - Shaders. For the text render shader program  (without the matrix transform) 
//---------------------------------------------------------------------------*/

// the vertex shader simply passes through data
const char VertexShader[] = "#version 150 core\n \
							#extension GL_ARB_explicit_attrib_location : enable\n \
							layout(location = 0) in vec4 a_Vertex;\n \
							layout(location = 1) in vec4 a_TexCoord0;\n \
							out vec4 v_TexCoordVs;\n \
							void main( void ) {\n \
								v_TexCoordVs = a_TexCoord0;\n \
								gl_Position = a_Vertex;\n \
							}\n";

// the geometry shader creates the billboard quads
const char GeometryShader[] = "#version 150 core\n \
							layout (points) in;\n \
							layout (triangle_strip, max_vertices = 4) out;\n \
							in vec4 v_TexCoordVs[];\n \
							out vec2 v_TexCoord0;\n \
							uniform vec2 u_Scale;\n \
							uniform vec4 u_Color;\n \
							out vec4 v_Color;\n \
							void main() {\n \
								v_Color = (u_Color / 255);\n \
								vec4 qv = gl_in[0].gl_Position;\n \
								vec4 tex = v_TexCoordVs [0];\n \
								gl_Position = vec4(-1 + (qv.x * u_Scale.x), 1 - (qv.y * u_Scale.y), 0, 1);\n \
								v_TexCoord0.xy = tex.xy;\n \
								EmitVertex();\n \
								gl_Position = vec4(-1 + (qv.z * u_Scale.x), 1 - (qv.y * u_Scale.y), 0, 1);\n \
								v_TexCoord0.x = tex.x + tex.z;\n \
								v_TexCoord0.y = tex.y;\n \
								EmitVertex();\n \
								gl_Position = vec4(-1 + (qv.x * u_Scale.x), 1 - (qv.w * u_Scale.y), 0, 1);\n \
								v_TexCoord0.x = tex.x;\n \
								v_TexCoord0.y = tex.y + tex.w;\n \
								EmitVertex();\n \
								gl_Position = vec4(-1 + (qv.z * u_Scale.x), 1 - (qv.w * u_Scale.y), 0, 1);\n \
								v_TexCoord0.x = tex.x + tex.z;\n \
								v_TexCoord0.y = tex.y + tex.w;\n \
								EmitVertex();\n \
								EndPrimitive();\n \
							}\n";

// the fragment shader
const char FragmentShader[] = "#version 150 core\n \
							uniform sampler2D textureUnit0;\n \
							in vec2 v_TexCoord0;\n \
							in vec4 v_Color;\n \
							out vec4 FragColor;\n \
							void main( void ) {\n \
								FragColor = vec4(1, 1, 1, texture(textureUnit0, v_TexCoord0).r ) * v_Color;\n \
							}\n";
