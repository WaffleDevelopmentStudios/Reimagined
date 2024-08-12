#version 450 core
layout (location = 0) out vec4      FragColor;
layout (location = 0) in  float     inBlockWooden;
layout (location = 1) in  float     inBlockState;
layout (location = 2) in  float     inBlockFace;
layout (location = 3) in  float     inBlockItem;
layout (location = 4) in  vec2      inTexCoord;
                 uniform  sampler2D TerrainAtlas;
				 uniform  sampler2D ItemAtlas;

// Block ID's
#define Dirt                 1
#define GrassBlock           2
#define Stone                3
#define Cobblestone          4
#define StoneBricks          5
#define PolishedStone        6
#define PolishedStoneBricks  7
#define Log                  8
#define Planks               9
#define Sand                 10
#define Sandstone            11
#define Bedrock              12
#define Bookshelf            13
#define Workbench            14
#define Bricks               15
#define Jukebox              16
#define Furnace              17
#define Glass                18

// Item ID's
#define WoodenSword     1
#define WoodenShovel    2
#define WoodenPickaxe   3
#define WoodenAxe       4
#define WoodenHoe       5
#define StoneSword      6
#define StoneShovel     7
#define StonePickaxe    8
#define StoneAxe        9
#define StoneHoe        10
#define IronSword       11
#define IronShovel      12
#define IronPickaxe     13
#define IronAxe         14
#define IronHoe         15
#define GoldSword       16
#define GoldShovel      17
#define GoldPickaxe     18
#define GoldAxe         19
#define GoldHoe         20
#define DiamondSword    21
#define DiamondShovel   22
#define DiamondPickaxe  23
#define DiamondAxe      24
#define DiamondHoe      25
#define SteelSword      26
#define SteelShovel     27
#define SteelPickaxe    28
#define SteelAxe        29
#define SteelHoe        30

/*
// Block Faces
0:  Front
1:  Back
2:  Top
3:  Bottom
4:  Left
5:  Right
*/
vec2 blockWoodenToTexture(float blockWooden, float blockFace) {
	if (blockWooden == Dirt) {
		return vec2(2, 0);
	} if (blockWooden == GrassBlock) {
		if (blockFace == 2)
			return vec2(0, 0);
		if (blockFace == 3)
			return vec2(2, 0);
		return vec2(3, 0);
	} if (blockWooden == Stone) {
		return vec2(1, 0);
	} if (blockWooden == Cobblestone) {
		return vec2(0, 1);
	} if (blockWooden == PolishedStone) {
		return vec2(4, 1);
	} if (blockWooden == Log) {
		if ((blockFace == 2) || (blockFace == 3))
			return vec2(2, 1);
		return vec2(1, 1);
	}
	if (blockWooden == Planks) {
		return vec2(3, 1);
	}
	if (blockWooden == Sand) {
		return vec2(0, 2);
	}
	if (blockWooden == Sandstone) {
		if (blockFace == 2)
			return vec2(0, 3);
		if (blockFace == 3)
			return vec2(0, 5);
		return vec2(0, 4);
	}
	if (blockWooden == PolishedStoneBricks) {
		return vec2(1, 2);
	}
	if (blockWooden == StoneBricks) {
		return vec2(1, 3);
	}
	if (blockWooden == Bedrock) {
		return vec2(4, 2);
	}
	if (blockWooden == Bookshelf) {
		if ((blockFace == 2) || (blockFace == 3))
			return vec2(3, 1);
		return vec2(4, 0);
	}
	if (blockWooden == Workbench) {
		if ((blockFace == 4) || (blockFace == 5))
			return vec2(2, 3);
		if ((blockFace == 0) || (blockFace == 1))
			return vec2(3, 3);
		if (blockFace == 2)
			return vec2(2, 2);
		return vec2(3, 2);
	} if (blockWooden == Bricks) {
		return vec2(4, 3);
	} if (blockWooden == Jukebox) {
		if (blockFace == 2)
			return vec2(2, 4);
		return vec2(1, 4);
	} if (blockWooden == Furnace) {
		if (blockFace == 0)
			return vec2(3, 4);
		if (blockFace == 2)
			return vec2(2, 5);
		if (blockFace == 3)
			return vec2(3, 5);
		return vec2(4, 4);
	} if (blockWooden == Glass) {
		return vec2(5, 1);
	}
	return vec2(31, 31);
}

vec2 itemWoodenToTexture(float blockWooden, float blockFace) {
	if (blockWooden == WoodenSword) {
		return vec2(0, 0);
	} if (blockWooden == WoodenShovel) {
		return vec2(0, 1);
	} if (blockWooden == WoodenPickaxe) {
		return vec2(0, 2);
	} if (blockWooden == WoodenAxe) {
		return vec2(0, 3);
	} if (blockWooden == WoodenHoe) {
		return vec2(0, 4);
	}
	return vec2(31, 31);
}

void main() {
	vec4 texturedColor;
	if (inBlockItem == 0) {
		texturedColor = texture(TerrainAtlas, vec2(inTexCoord.x + (blockWoodenToTexture(inBlockWooden, inBlockFace).x * 0.03125), inTexCoord.y + (blockWoodenToTexture(inBlockWooden, inBlockFace).y * 0.03125)));
	} else {
		texturedColor = texture(ItemAtlas, vec2(inTexCoord.x + (itemWoodenToTexture(inBlockWooden, inBlockFace).x * 0.03125), inTexCoord.y + (itemWoodenToTexture(inBlockWooden, inBlockFace).y * 0.03125)));
	}
	if(texturedColor.a < 0.1)
        discard;
	FragColor = texturedColor;
}
