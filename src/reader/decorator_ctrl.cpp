/*
 *  Copyright (c) 2021 Uskrai
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

#include "fmr/reader/decorator_ctrl.h"

#include "fmr/bitmap/bitmap_vector_event.h"

namespace fmr {

namespace reader {
void DecoratorCtrl::Create(ScrolledImageWindow *window,
                           bitmap::BitmapPageCtrl *bmp_page_ctrl) {
  page_indicator_ = std::make_unique<PageIndicator>();

  bitmap_page_ctrl_ = bmp_page_ctrl;
  bitmap_page_ctrl_->Bind(bitmap::kEventBitmapChanged,
                          &DecoratorCtrl::OnBitmapChanged, this);

  AddDecorator(page_indicator_.get());
  SetWindow(window);
}

void DecoratorCtrl::OnBitmapChanged(bitmap::BitmapVectorEvent &event) {
  page_indicator_->SetPage(event.GetPagePos() + 1);
  page_indicator_->SetPageLimit(bitmap_page_ctrl_->GetAllPage().size());
  page_indicator_->ShowOnce(500);
  page_indicator_->SetRect(wxPoint(0, 0), GetWindow()->GetClientSize());
  GetWindow()->Refresh();
  event.Skip();
}

ScrolledImageWindow *DecoratorCtrl::GetWindow() {
  return static_cast<ScrolledImageWindow *>(WindowDecoratorList::GetWindow());
}

void DecoratorCtrl::SetWindow(ScrolledImageWindow *window) {
  return WindowDecoratorList::SetWindow(window);
}

}  // namespace reader

}  // namespace fmr
