#version 450 core
layout (location = 0) in vec3  inPos;
layout (location = 1) in vec2  inTexCoord;
layout (location = 2) in float inBlockType;
layout (location = 3) in float inBlockState;
layout (location = 4) in float inBlockFace;
layout (location = 5) in float inBlockItem;

layout (location = 0) out float outBlockType;
layout (location = 1) out float outBlockState;
layout (location = 2) out float outBlockFace;
layout (location = 3) out float outBlockItem;
layout (location = 4) out vec2  outTexCoord;

uniform mat4 projection;
uniform float fixedProjection;

float shrinkFactor = 0.25;

void main() { 
	float xPos = (inPos.x * shrinkFactor) + (1 - shrinkFactor);
	float yPos = ((inPos.y * fixedProjection) * shrinkFactor) - 1;
	gl_Position = vec4(xPos, yPos, inPos.z, 1.0);
	outTexCoord = vec2(inTexCoord.x * 0.03125, mix(1, 0, inTexCoord.y) * 0.03125);
	outBlockType = inBlockType;
	outBlockState = inBlockState;
	outBlockFace = inBlockFace;
	outBlockItem = inBlockItem;

}
