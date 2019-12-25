#ifndef _CMG_GRAPHICS_UTILITIES_SHELF_BIN_PACKER_H_
#define _CMG_GRAPHICS_UTILITIES_SHELF_BIN_PACKER_H_

#include <cmgCore/containers/cmgArray.h>

namespace cmg {

/*
   SHELF BIN-PACKING ALGORITHM

 ##############################
 #......#.....#...#....#....# #
 #......#.....#...#....#....# #
 #......#.....#...#....#....# #
 #......#.....#...#....###### #
 #......#.....#...#....#      #
 #......#.....##########      #
 #......#######               #
 #......#                     #
 ##############################
 #.....#...#.....#....#...#...#
 #.....#...#.....#....#########
 #.....#...############       #
 #.....#...#                  #
 ##############################

*/

// This class is used to pack rectangles into a bin, using
// the shelf packing algorithm. This is helpful for
// packing sprites into a sprite sheet, and is currently
// being used to pack several glyph images for a font
// into a single texture.
class ShelfBinPacker
{
public:
	ShelfBinPacker();

	void Insert(int32* x, int32* y, int32 width, int32 height, int32 padding = 0);
	void Pack(int32* binWidth, int32* binHeight);
	void Clear();

private:
	struct BinRect
	{
		int32* pX;
		int32* pY;
		int32 width;
		int32 height;
		int32 padding;
	};

	Array<BinRect> m_rects;
	int32 m_rectAreaSum;
};


}

#endif // _CMG_GRAPHICS_UTILITIES_SHELF_BIN_PACKER_H_