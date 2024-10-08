#version 450 core
layout (location = 0) in vec3  inPos;
layout (location = 1) in vec2  inTexCoord;
layout (location = 2) in float inBlockType;
layout (location = 3) in float inBlockState;
layout (location = 4) in float inBlockFace;

layout (location = 0) out float outBlockType;
layout (location = 1) out float outBlockState;
layout (location = 2) out float outBlockFace;
layout (location = 3) out vec2  outTexCoord;


uniform mat4 view;
uniform mat4 projection;
uniform vec4 chunkOffset;

void main() {

	gl_Position = projection * view * (vec4(inPos, 1.0f) + ((chunkOffset) * 16));
	outTexCoord = vec2(inTexCoord.x * 0.03125, inTexCoord.y * 0.03125);
	outBlockType = inBlockType;
	outBlockState = inBlockState;
	outBlockFace = inBlockFace;

}

