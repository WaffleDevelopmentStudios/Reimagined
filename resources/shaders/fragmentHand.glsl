#version 450 core
layout (location = 0) out vec4      FragColor;
layout (location = 0) in  float     inBlockType;
layout (location = 1) in  float     inBlockState;
layout (location = 2) in  float     inBlockFace;
layout (location = 3) in  vec2      inTexCoord;
                 uniform  sampler2D Texture;

#define Dirt            1
#define GrassBlock      2
#define Stone           3
#define Cobblestone     4
#define StoneBricks     5
#define StoneTiles      6
#define PolishedStone   7
#define Log             8
#define Planks          9
#define Sand            10
#define Sandstone       11
#define Bedrock         12
#define Bookshelf       13
#define Workbench       14
#define Bricks          15
#define Jukebox         16
#define Furnace         17

/*
// Block Faces
0:  Front
1:  Back
2:  Top
3:  Bottom
4:  Left
5:  Right
*/
vec2 blockTypeToTexture(float blockType, float blockFace) {
	if (blockType == Dirt) {
		return vec2(2, 0);
	} if (blockType == GrassBlock) {
		if (blockFace == 2)
			return vec2(0, 0);
		if (blockFace == 3)
			return vec2(2, 0);
		return vec2(3, 0);
	} if (blockType == Stone) {
		return vec2(1, 0);
	} if (blockType == Cobblestone) {
		return vec2(0, 1);
	} if (blockType == PolishedStone) {
		return vec2(4, 1);
	} if (blockType == Log) {
		if ((blockFace == 2) || (blockFace == 3))
			return vec2(2, 1);
		return vec2(1, 1);
	}
	if (blockType == Planks) {
		return vec2(3, 1);
	}
	if (blockType == Sand) {
		return vec2(0, 2);
	}
	if (blockType == Sandstone) {
		if (blockFace == 2)
			return vec2(0, 3);
		if (blockFace == 3)
			return vec2(0, 5);
		return vec2(0, 4);
	}
	if (blockType == StoneBricks) {
		return vec2(1, 2);
	}
	if (blockType == StoneTiles) {
		return vec2(1, 3);
	}
	if (blockType == Bedrock) {
		return vec2(4, 2);
	}
	if (blockType == Bookshelf) {
		if ((blockFace == 2) || (blockFace == 3))
			return vec2(3, 1);
		return vec2(4, 0);
	}
	if (blockType == Workbench) {
		if ((blockFace == 4) || (blockFace == 5))
			return vec2(2, 3);
		if ((blockFace == 0) || (blockFace == 1))
			return vec2(3, 3);
		if (blockFace == 2)
			return vec2(2, 2);
		return vec2(3, 2);
	} if (blockType == Bricks) {
		return vec2(4, 3);
	} if (blockType == Jukebox) {
		if (blockFace == 2)
			return vec2(2, 4);
		return vec2(1, 4);
	} if (blockType == Furnace) {
		if (blockFace == 0)
			return vec2(3, 4);
		if (blockFace == 2)
			return vec2(2, 5);
		if (blockFace == 3)
			return vec2(3, 5);
		return vec2(4, 4);
	}
	return vec2(31, 31);
}

void main() {
	vec4 shadedColor = texture(Texture, vec2(inTexCoord.x + (blockTypeToTexture(inBlockType, inBlockFace).x * 0.03125), inTexCoord.y + (blockTypeToTexture(inBlockType, inBlockFace).y * 0.03125)));
	FragColor = shadedColor;
}
