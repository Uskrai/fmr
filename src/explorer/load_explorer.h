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

#ifndef FMR_EXPLORER_LOAD_EXPLORER
#define FMR_EXPLORER_LOAD_EXPLORER

#include "thread/thread.h"
#include "explorer/common.h"

#include <queue>

namespace fmr
{

namespace explorer
{

class LoadThread
    : public BaseThread
{
    public:
        LoadThread( ThreadController *parent, wxThreadKind kind = wxTHREAD_DETACHED, int id = wxID_ANY )
        : BaseThread( parent, kind, id ) {};

        ExitCode Entry();
        void Load( StreamBitmap &stream );

        void SetSize( const wxSize &size );
        void SetImageQuality( wxImageResizeQuality quality );
        void Clear();

        void Push( StreamBitmap &stream_bitmap );
    private:
        std::queue<StreamBitmap> load_queue_;

        wxSize image_size_;
        wxImageResizeQuality image_quality_ = wxIMAGE_QUALITY_NORMAL;
};

}; // namespace explorer

}; // namespace fmr

#endif