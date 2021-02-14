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

#ifndef FMR_QUEUE_EVENT
#define FMR_QUEUE_EVENT

#include <fmr/queue/base.h>
#include <wx/event.h>

#include <memory>

namespace fmr {

namespace queue {

template <typename ItemClass>
class ItemEvent : public wxEvent {
  ItemClass item_;

 public:
  ItemEvent(int id, wxEventType type, ItemClass item)
      : wxEvent(id, type), item_(std::move(item)) {}
  ItemEvent(const ItemEvent &other) : wxEvent(other), item_(other.item_) {}
  ItemEvent(ItemEvent &&other) = default;

  ItemClass &GetItem() { return item_; }
  const ItemClass &GetItem() const { return item_; }

  void SetItem(ItemClass item) { item_ = std::move(item); }

  virtual wxEvent *Clone() const override { return new ItemEvent(*this); }
};

template <typename ItemClass>
class ItemReceiverEvent : public ItemReceiver<ItemClass> {
  wxEvtHandler *parent_ = nullptr;
  wxEventType event_type_;
  int event_id_ = wxID_ANY;

 public:
  ItemReceiverEvent(wxEvtHandler *parent, int event_id) {
    SetParent(parent);
    SetEventId(event_id);
  }

  void TakeItem(ItemClass &&item) override {
    auto event = CreateEvent(std::move(item));
    wxQueueEvent(parent_, event);
  }

  virtual wxEvent *CreateEvent(ItemClass &&item) {
    return new ItemEvent<ItemClass>(GetEventId(), GetEventType(item),
                                    std::move(item));
  }

  void SetEventId(int id) { event_id_ = id; }
  int GetEventId() const { return event_id_; }

  virtual wxEventType GetEventType(ItemClass &item) const {
    return event_type_;
  }
  void SetEventType(wxEventType type) { event_type_ = type; }
  void SetParent(wxEvtHandler *parent) { parent_ = parent; }
};

}  // namespace queue

}  // namespace fmr

#endif /* end of include guard: FMR_QUEUE_EVENT */
