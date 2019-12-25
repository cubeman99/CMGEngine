#include "cmgShelfBinPacker.h"

namespace cmg {


ShelfBinPacker::ShelfBinPacker() :
	m_rectAreaSum(0)
{
}

// Insert a rect into the bin, passing in pointers to where the rect position will by modified.
void ShelfBinPacker::Insert(int* pX, int* pY, int width, int height, int padding)
{
	BinRect r;
	r.pX      = pX;
	r.pY      = pY;
	r.width   = width;
	r.height  = height;
	r.padding = padding;

	// Insert rect into the list, sorted by height.
	uint32 i;
	for (i = 0; i < m_rects.size(); i++)
	{
		if (r.height + (2 * r.padding) >= m_rects[i].height + (2 * m_rects[i].padding))
			break;
	}
	m_rects.insert(m_rects.begin() + i,  r);
	m_rectAreaSum += (width + (2 * padding)) * (height + (2 * padding));
}


// Clear all the rects.
void ShelfBinPacker::Clear()
{
	m_rects.clear();
	m_rectAreaSum = 0;
}


// Pack all the rects, modifying their positions, and output the bin dimensions.
void ShelfBinPacker::Pack(int* binWidth, int* binHeight)
{
	// Determine the minimum power-of-two bin size from the area sum of all rects.
	int size = 32;
	while (size * size < m_rectAreaSum)
		size *= 2;

	int x = 0;
	int y = 0;
	int shelfHeight = 0;
	bool isTooSmall = true;

	while (isTooSmall)
	{
		isTooSmall = false;

		// Pack the rects into shelves.
		for (uint32 i = 0; i < m_rects.size(); i++)
		{
			BinRect& r = m_rects[i];
			int w = r.width  + (2 * r.padding);  // Width including padding.
			int h = r.height + (2 * r.padding);  // Width including padding.
			
			// Have we reached the right edge of the bin?
			if (x + w > size)
			{
				// Move on to the next shelf.
				x = 0;
				y += shelfHeight;
				shelfHeight = h;

				// Have we run out of vertical shelf space?
				if (y + h > size)
				{
					isTooSmall = true;
					break;
				}
			}
			
			if (h > shelfHeight)
				shelfHeight = h;

			// Output the packed position of the rect.
			if (r.pX != NULL)
				*r.pX = x + r.padding;
			if (r.pY != NULL)
				*r.pY = y + r.padding;

			x += w;
		}

		// Double the size if it was too small.
		if (isTooSmall)
			size *= 2;
	}

	// Output the final bin dimensions.
	if (binWidth != NULL)
		*binWidth  = size;
	if (binHeight != NULL)
		*binHeight = size;
}

}
