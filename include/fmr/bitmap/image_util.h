/*
 *  Copyright (c) 2020 Uskrai
 *  
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef FMR_BITMAP_IMAGE_UTIL
#define FMR_BITMAP_IMAGE_UTIL

#include <wx/image.h>
#include <fmr/bitmap/bmp.h>
#include <fmr/handler/struct_stream.h>
#include <fmr/handler/abstract_handler.h>

namespace fmr
{

/**
 * @brief image related operation like load,zoom,save to file.
 *
 */

namespace image_util
{

    /**
     * @brief load image from input stream
     * 
     * @param image target image to place the image from stream
     * @param stream input stream to load the image from
     * 
     * @return true if loaded
     * @return false if not loaded
     */
    bool Load( wxImage &image, wxInputStream &stream );
    /**
     * @param stream struct_stream that contain the input stream
     */
    bool Load( wxImage &image, const SStream &stream );
    /**
     * @param handler handler to get the stream from
     * @param index the index of the struct_stream
     */
    bool Load( wxImage &image, const AbstractHandler &handler, size_t index );

    /**
     * @name Rescaling Image
     * @return true if zoomed
     * @return false
     */
    //@{
    /**
     * @param image the original and rescaled target
     * @param scale factor to scale image
     * @param image_quality see wxImage
     * @return true 
     * @return false 
     */
    bool Rescale( wxImage &image, float scale, wxImageResizeQuality image_quality = wxIMAGE_QUALITY_NORMAL );

    /**
     * @param bmp target bitmap to set the image to
     * @param original_image Original image to resize
     */
    bool Rescale( SBitmap &bmp, wxImage &original_image, float scale, wxImageResizeQuality image_qualitry = wxIMAGE_QUALITY_NORMAL );

    /**
     * @brief rescale image while retaining aspect ratio
     * this will ignore size with negative value
     *
     * @param image 
     * @param maximum_size maximum page for the image to resize to
     * 
     */
    bool Rescale( wxImage &image, wxSize maximum_size, wxImageResizeQuality image_quality = wxIMAGE_QUALITY_NORMAL );
    //@}
}




}; // namespace fmr

#endif
