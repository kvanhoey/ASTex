/*******************************************************************************
* ASTex:                                                                       *
* Copyright (C) IGG Group, ICube, University of Strasbourg, France             *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Web site: https://astex-icube.github.io                                      *
* Contact information: astex@icube.unistra.fr                                  *
*                                                                              *
*******************************************************************************/



#ifndef __ASTEX__CONNECTED_COMPONENTS_H__
#define __ASTEX__CONNECTED_COMPONENTS_H__




#include "region_growing.h"




namespace ASTex
{


template <typename PixelType>
struct ConnectedComponent
{
    uint32_t                            id;
    PixelType                           label;
    std::vector< itk::Index<2> >        pixels;
    std::vector< ConnectedComponent* >  neighbors;
};


template < typename NeighborhoodT,
           typename TImage >
void getConnectedComponents( const TImage &img, std::vector< ConnectedComponent< typename TImage::PixelType> > &connectedComponents )
{
	using CC = ConnectedComponent<typename TImage::PixelType >;


    // Extract the list of connected components that exist in the given picture.
    // A connected component is a region of contiguous pixels having the same value.

    connectedComponents.clear();

    RegionGrowingOperator< NeighborhoodT > regionOp( img );
    auto seed = img.beginConstIterator();

    do
    {
        if( !regionOp.isAssignedToARegion(seed.GetIndex()) )
        {
            connectedComponents.push_back( CC() );
            CC &cc = connectedComponents.back();

            cc.id    = connectedComponents.size() - 1;
            cc.label = seed.Value();

			regionOp.template newRegionFromSeed< GrowingStrategyRegular >(
                seed.GetIndex(),

                // Condition function for accepting a pixel to the current region.
                [&]( const itk::Index<2> &pixel )
                {
                    return img.pixelAbsolute(pixel) == cc.label;
                },

                // Processing function applied to accepted pixels.
				[&]( const itk::Index<2> &pixel, const typename NeighborhoodT::NeighborsList &neighbors )
                {
                    cc.pixels.push_back( pixel );
					std::ignore = neighbors;
                }
            );
        }

        ++ seed;
    } while( !seed.IsAtEnd() );
}


template < typename NeighborhoodT,
           typename TImage >
void getConnectedComponentsWithNeighborhood( const TImage &img, std::vector< ConnectedComponent< typename TImage::PixelType> > &connectedComponents )
{
	using CC = ConnectedComponent<typename TImage::PixelType >;


    // Extract the list of connected components that exist in the given picture.
    // A connected component is a region of contiguous pixels having the same value.

    connectedComponents.clear();

    RegionGrowingOperator< NeighborhoodT > regionOp( img );
    auto seed = img.beginConstIterator();

    ASTex::ImageGrayu32 ccIdMap( img.width(), img.height() );
    std::vector< std::vector<itk::Index<2>> > neighboringPixels;

    do
    {
        if( !regionOp.isAssignedToARegion(seed.GetIndex()) )
        {
            connectedComponents.push_back( CC() );
            CC &cc = connectedComponents.back();

            cc.id    = connectedComponents.size() - 1;
            cc.label = seed.Value();

            neighboringPixels.push_back( std::vector<itk::Index<2>>() );

			regionOp.template newRegionFromSeed< GrowingStrategyRegular >(
                seed.GetIndex(),

                // Condition function for accepting a pixel to the current region.
                [&]( const itk::Index<2> &pixel )
                {
                    return img.pixelAbsolute(pixel) == cc.label;
                },

                // Processing function applied to accepted pixels.
				[&]( const itk::Index<2> &pixel, const typename NeighborhoodT::NeighborsList &neighbors )
                {
                    ccIdMap.pixelAbsolute( pixel ) = cc.id;
                    cc.pixels.push_back( pixel );

                    for( auto nn : neighbors )
                        if( img.pixelAbsolute(nn) != cc.label )
                            neighboringPixels[cc.id].push_back( nn );
                }
            );
        }

        ++ seed;
    } while( !seed.IsAtEnd() );


    std::vector<bool> isNeighborOf;
    isNeighborOf.assign( connectedComponents.size(), false );

    for( auto &cc : connectedComponents )
    {
        for( auto &nn : neighboringPixels[cc.id] )
        {
            auto nnId = ccIdMap.pixelAbsolute( nn );
            if( !isNeighborOf[nnId] )
            {
                cc.neighbors.push_back( &connectedComponents[nnId] );
                isNeighborOf[nnId] = true;
            }
        }

        for( auto nn : cc.neighbors )
            isNeighborOf[nn->id] = false;
    }
}


} // namespace ASTex




#endif // __ASTEX__CONNECTED_COMPONENTS_H__
