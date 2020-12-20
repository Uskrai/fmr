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

#include "thread/thread.h"
#include "bitmap/bitmap.h"
#include "handler/abstract_handler.h"
#include "reader/load_reader.h"
#include "reader/zoom_reader.h"

#include <memory>

namespace fmr
{

namespace reader
{

enum ControllerExit
{
    kControllerPathEmpty,
    kControllerBitmapEmpty,
    kControllerHandlerNotFound,
    kControllerFolderEmpty,
    kControllerCantRunThread,
    kControllerSuccess
};

enum ControllerZoom
{
    kControllerZoomPointed,
    kControllerZoomPage,
    kControllerZoomAll
};

enum ReaderThreadID
{
    kLoadThreadID = wxID_HIGHEST + + 30,
    kZoomThreadID
};

class Controller
    : public ThreadController
{
    public:
        Controller( wxWindow *parent );
        ~Controller();

        ControllerExit Open( const wxString &path );

        void Zoom( const wxPoint &area, const wxPoint &pos, const ControllerZoom &flags );

        void SetBitmap( std::shared_ptr<Bitmap> bitmap );
        void SetHandler( std::shared_ptr<AbstractHandler> handler );


        std::shared_ptr<Bitmap> GetBitmap() { return bitmap_; }
        std::shared_ptr<AbstractHandler> GetHandler() { return handler_; }

        std::shared_ptr<const Bitmap> GetBitmap() const { return bitmap_; }
        std::shared_ptr<const AbstractHandler> GetHandler() const { return handler_; }

        wxEvtHandler *GetParent(){ return parent_; }
        wxThread *GetThread( int id );
        void DoSetNull( int id );
    
    protected:
        wxWindow *parent_ = NULL;
        std::shared_ptr<AbstractHandler> handler_ = NULL;
        std::shared_ptr<Bitmap> bitmap_ = NULL;

        LoadThread *load_thread_ = NULL;
        ZoomThread *zoom_thread_ = NULL;

}; // class Controller

}; // nmamespace reader

}; // namespace fmr