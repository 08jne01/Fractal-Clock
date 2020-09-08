#version 460 core
//Input Buffer
layout( location = 0 ) in vec3 vertexPositions;
layout( location = 1 ) in vec3 clockSettings;
//clockSettings
//x: size (length of the hands second hand )
//y: frac (level of the clock + 1) / (maximum number of levels + 1) this is 0 -> 1
// Levels increase as clock gets smaller.
//z: line width

//sin(x)
//cos(x)
//pow(x,y) -> x^y


#define PI 3.14159265358979

//Output
out float v_size;
out float v_width;
out float v_rotation;
out vec4 v_vertexColor;

//Input time
uniform vec3 timeAngle; //hour: x, minute: y, seconds: z

vec4 getVertexColor()
{
	vec4 color; //r, g, b, alpha

	//Example 1
	color.x = clockSettings.y * pow( cos( -3.0 * timeAngle.y + PI * 0.75 ), 2 );
	color.y = clockSettings.y * pow( sin( timeAngle.y + timeAngle.x - clockSettings.y * PI ), 2 );
	color.z = pow( sin( 1.0 * timeAngle.y - PI ), 2 );
	color.w = pow( 1.0 - clockSettings.y, 0.5 );

	//make sure all components of color
	//stay between 0 -> 1
	//0 is no color
	//1 is max color

	//Example 2
	//color.x = timeAngle.z;
	//color.y = 0.0;
	//color.z = 0.0;
	//color.w = 1.0;

	return color;
}

void main()
{
	gl_Position = vec4( vertexPositions.xy, 0.0, 1.0 );
	
	v_size = clockSettings.x;
	v_rotation = vertexPositions.z;

	v_width = clockSettings.z;
	v_vertexColor = getVertexColor();
}