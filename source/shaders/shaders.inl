
//---------------------------------------------------------------------------*/
// GLSL - Shader. For the text render (without the matrix transform) 
//---------------------------------------------------------------------------*/
const char VertexShader[] = "#version 150 core	\n	\
							precision highp float;	\n	\
							#extension GL_ARB_explicit_attrib_location : enable	\n	\
							#define ATTRIB_POSITION  0	\n	\
							layout(location = ATTRIB_POSITION) in vec4 g_Vertex;	\n	\
							uniform vec2 u_Scale;	\n	\
							uniform vec4 u_Color;	\n	\
							out vec2 v_TexCoord0;	\n	\
							out vec4 v_Color;	\n	\
							void main( void ) {	\n	\
							gl_Position = vec4(-1 + (g_Vertex.x * u_Scale.x), 1 - (g_Vertex.y * u_Scale.y), 0, 1);	\n		\
							v_Color = (u_Color / 255);	\n	\
							v_TexCoord0 = g_Vertex.zw;	\n	\
							}	\n";

const char FragmentShader[] = "#version 150 core	\n	\
							  uniform sampler2D textureUnit0;	\n	\
							  in vec2 v_TexCoord0;	\n	\
							  in vec4 v_Color;	\n	\
							  out vec4 FragColor;	\n	\
							  void main( void ) {	\n	\
							  FragColor = vec4(1, 1, 1, texture(textureUnit0, v_TexCoord0).r ) * v_Color;	\n	\
							  }	\n";
