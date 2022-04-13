/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "components/root_view.h"

#include "common/screen.h"
#include "core/render_manager.h"
#include "dock/screen_device_proxy.h"
#include "gfx_utils/graphic_log.h"
#if ENABLE_WINDOW
#include "window/window_impl.h"
#endif
namespace OHOS {
namespace {
#if LOCAL_RENDER
const constexpr uint8_t MAX_SPLIT_NUM = 32; // split at most 32 parts
// view along with its parents and siblings are at most 128
const constexpr uint8_t VIEW_STACK_DEPTH = COMPONENT_NESTING_DEPTH * 2;
#else
const constexpr uint8_t VIEW_STACK_DEPTH = COMPONENT_NESTING_DEPTH;
#endif
static Rect g_maskStack[COMPONENT_NESTING_DEPTH];
static UIView* g_viewStack[VIEW_STACK_DEPTH];
} // namespace
RootView::RootView()
{
#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_init(&lock_, nullptr);
#endif
}

#if ENABLE_WINDOW
Window* RootView::GetBoundWindow() const
{
    return boundWindow_;
}
#endif

inline bool RootView::IntersectScreenRect(Rect& rect)
{
#if ENABLE_WINDOW
    Rect screenRect = GetRect();
#else
    Rect screenRect(0, 0, Screen::GetInstance().GetWidth() - 1, Screen::GetInstance().GetHeight() - 1);
#endif
    return rect.Intersect(rect, screenRect);
}

#if LOCAL_RENDER
using namespace Graphic;
static void DivideInvalidateRect(const Rect& originRect, Rect& leftoverRect, Vector<Rect>& splitRects)
{
    Rect mask;
    if (!mask.Intersect(originRect, leftoverRect)) {
        splitRects.PushBack(leftoverRect);
        return;
    }

    /*
     *   +---+---+---+
     *   |   | A |   |      originRect           :A+B
     *   |   +---+   |      leftoverRect         :A->0
     *   |     B     |      mask                 :A
     *   +-----------+
     */
    if (originRect.IsContains(leftoverRect)) {
        return;
    }

    int16_t reserveCnt = MAX_SPLIT_NUM - splitRects.Size();
    if (reserveCnt <= 0) {
        splitRects.PushBack(leftoverRect);
        return;
    }

    if (mask.GetWidth() == leftoverRect.GetWidth()) {
        /*
         *       +---+
         *       | A |        originRect           :B+C
         *   +-----------+    leftoverRect         :A+B->A
         *   |   | B |   |    mask                 :B
         *   |   +---+   |
         *   |     C     |
         *   +-----------+
         */
        if (mask.GetBottom() == leftoverRect.GetBottom()) {
            leftoverRect.SetBottom(mask.GetTop() - 1);
        } else if (mask.GetTop() == leftoverRect.GetTop()) {
            leftoverRect.SetTop(mask.GetBottom() + 1);
        } else {
            splitRects.PushBack(leftoverRect);
            splitRects.Back().SetBottom(mask.GetTop() - 1);
            leftoverRect.SetTop(mask.GetBottom() + 1);
        }
        splitRects.PushBack(leftoverRect);
        return;
    }
    if (mask.GetHeight() == leftoverRect.GetHeight()) {
        /*
         *      +---------+   originRect           :B+C
         *  +-------+     |   leftoverRect         :A+B->A
         *  | A | B |  C  |   mask                 :B
         *  +-------+     |
         *      +---------+
         */
        if (mask.GetLeft() == leftoverRect.GetLeft()) {
            leftoverRect.SetLeft(mask.GetRight() + 1);
        } else if (mask.GetRight() == leftoverRect.GetRight()) {
            leftoverRect.SetRight(mask.GetLeft() - 1);
        } else {
            splitRects.PushBack(leftoverRect);
            splitRects.Back().SetRight(mask.GetLeft() - 1);
            leftoverRect.SetLeft(mask.GetRight() + 1);
        }
        splitRects.PushBack(leftoverRect);
        return;
    }

    Vector<Rect> copyRect(splitRects);
    if (mask.GetLeft() != leftoverRect.GetLeft()) {
        /*
         *     |
         * +-------+
         * |   +---+   mask                 :A
         * | B | A |   leftoverRect         :A+B
         * |   +---+
         * +-------+
         *     |
         */
        if (reserveCnt-- <= 0) {
            splitRects.Swap(copyRect);
            splitRects.PushBack(leftoverRect);
            return;
        }
        splitRects.PushBack(leftoverRect);
        splitRects.Back().SetRight(mask.GetLeft() - 1);
        leftoverRect.SetLeft(mask.GetLeft());
    }

    if (mask.GetTop() != leftoverRect.GetTop()) {
        /*
         *  +-------+
         *  |   B   |   mask                 :A
         * ---+---+---  leftoverRect         :A+B
         *  | | A | |
         *  +-+---+-+
         */
        if (reserveCnt-- <= 0) {
            splitRects.Swap(copyRect);
            splitRects.PushBack(leftoverRect);
            return;
        }
        splitRects.PushBack(leftoverRect);
        splitRects.Back().SetBottom(mask.GetTop() - 1);
        leftoverRect.SetTop(mask.GetTop());
    }

    if (mask.GetRight() != leftoverRect.GetRight()) {
        /*
         *     |
         * +-------+
         * +---+   |    mask                 :A
         * | A | B |    leftoverRect         :A+B
         * +---+   |
         * +-------+
         *     |
         */
        if (reserveCnt-- <= 0) {
            splitRects.Swap(copyRect);
            splitRects.PushBack(leftoverRect);
            return;
        }
        splitRects.PushBack(leftoverRect);
        splitRects.Back().SetLeft(mask.GetRight() + 1);
        leftoverRect.SetRight(mask.GetRight());
    }

    if (mask.GetBottom() != leftoverRect.GetBottom()) {
        /*
         *  +-+---+-+
         *  | | A | |     mask                 :A
         * ---+---+---    leftoverRect         :A+B
         *  |   B   |
         *  +-------+
         */
        if (reserveCnt-- <= 0) {
            splitRects.Swap(copyRect);
            splitRects.PushBack(leftoverRect);
            return;
        }
        splitRects.PushBack(leftoverRect);
        splitRects.Back().SetTop(mask.GetBottom() + 1);
        leftoverRect.SetBottom(mask.GetBottom());
    }
    return;
}

static void AddRenderedRects(Rect& rect, List<Rect>& renderedRects, ListNode<Rect>* iter)
{
    /* Elements at front have larger area and more relevance */
    for (; iter != renderedRects.End(); iter = iter->next_) {
        Rect& curRect = iter->data_;
        if (!curRect.IsIntersect(rect)) {
            continue;
        }

        if (curRect.IsContains(rect)) {
            return;
        }

        /* Merge two rects */
        if (rect.IsContains(curRect)) {
        } else if (((curRect.GetLeft() == rect.GetLeft()) && (curRect.GetRight() == rect.GetRight())) ||
                   ((curRect.GetTop() == rect.GetTop()) && (curRect.GetBottom() == rect.GetBottom()))) {
            rect.Join(curRect, rect);
        } else {
            continue;
        }
        iter = renderedRects.Remove(iter)->prev_;
        break;
    }
    if (iter == renderedRects.End()) {     // No merge rises
        if (renderedRects.Size() == 128) { // record 128 rendered rects at most
            renderedRects.PopBack();
        }
        renderedRects.PushFront(rect);
    } else { // merge rises, go over the rest nodes
        AddRenderedRects(rect, renderedRects, iter);
    }
}

void RootView::RemoveViewFromInvalidMap(UIView* view)
{
#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_lock(&lock_);
#endif

    int16_t stackCount = 0;
    do {
        while (view != nullptr) {
            /* delete node itself */
            auto entry = invalidateMap_.find(view);
            if (entry != invalidateMap_.end()) {
                invalidateMap_.erase(entry);
            }
            /* delete node's children */
            if (view->IsViewGroup() && stackCount < COMPONENT_NESTING_DEPTH) {
                g_viewStack[stackCount++] = view;
                view = static_cast<UIViewGroup*>(view)->GetChildrenHead();
                continue;
            }
            /* only go to child's sibling */
            view = view->GetNextSibling();
        }
        if (--stackCount >= 0) {
            view = g_viewStack[stackCount]->GetNextSibling();
        }
    } while (stackCount >= 0);

#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_unlock(&lock_);
#endif
}

void RootView::OptimizeInvalidView(UIView* curview, UIView* background, List<Rect>& renderedRects)
{
    if (curview == nullptr) {
        return;
    }
    auto mapEntry = invalidateMap_.find(curview);
    if (mapEntry == invalidateMap_.end()) {
        return;
    }

    Rect& invalidRect = mapEntry->second.Front();
    /* Transparent views should draw from background */
    if (((curview->GetStyleConst().bgOpa_ != OPA_OPAQUE) || (curview->GetOpaScale() != OPA_OPAQUE) ||
        (!curview->IsTransInvalid())) &&
        (curview != this)) {
        AddInvalidateRect(invalidRect, background);
        invalidateMap_.erase(mapEntry);
        return;
    }

    /* Remove the rendered parts and split the origin rect into splitInvalidRects
     * For performance reason, split numbers are strictly restrained.
     */
    Vector<Rect> splitInvalidRects(MAX_SPLIT_NUM << 1);
    Rect invalidRectCopy(invalidRect);
    /* Using forward order because entries at the front are closer to the current view and have larger Size */
    for (auto iter = renderedRects.Begin(); iter != renderedRects.End(); iter = iter->next_) {
        for (int8_t i = 0; i < mapEntry->second.Size(); i++) {
            DivideInvalidateRect(iter->data_, mapEntry->second[i], splitInvalidRects);
        }
        mapEntry->second.Swap(splitInvalidRects);
        splitInvalidRects.Clear();
    }

    /* Add new opaque rects */
    Rect preDrawRect(invalidRectCopy);
    if (!curview->OnPreDraw(preDrawRect)) {
        AddInvalidateRect(invalidRectCopy, background);
    }
    AddRenderedRects(preDrawRect, renderedRects, renderedRects.Begin());
}

void RootView::OptimizeInvalidMap()
{
    UIView* curview = this;
    int16_t stackCount = 0;
    int16_t opaStackCount = 0;
    UIView* background[VIEW_STACK_DEPTH];
    bool flags[VIEW_STACK_DEPTH]; // indicate whether stack go back from child
    List<Rect> renderedRects;     // Record rendered areas to avoid rerendering

    do {
        /* push stack */
        if (curview != nullptr) {
            if (stackCount >= VIEW_STACK_DEPTH) {
                return;
            }
            g_viewStack[stackCount] = curview;
            flags[stackCount++] = false;
            curview = curview->GetNextSibling();
            continue;
        }

        curview = g_viewStack[--stackCount];
        Rect rect(curview->GetRect());
        if (!curview->IsVisible() || !IntersectScreenRect(rect)) {
            curview = nullptr;
            continue;
        }
        if (!flags[stackCount]) { // Back from sibling
            if (curview->IsViewGroup()) {
                /* Set background/topview */
                if (((curview->GetStyleConst().bgOpa_ == OPA_OPAQUE) && (curview->GetOpaScale() == OPA_OPAQUE) &&
                    curview->IsTransInvalid()) ||
                    (curview == this)) {
                    background[opaStackCount] = curview;
                } else {
                    background[opaStackCount] = background[opaStackCount - 1];
                }
                ++opaStackCount;
                if (opaStackCount >= VIEW_STACK_DEPTH) {
                    return;
                }
                flags[stackCount++] = true;
                curview = static_cast<UIViewGroup*>(curview)->GetChildrenHead();
                continue;
            }
        } else { // Back from child
            opaStackCount--;
        }
        OptimizeInvalidView(curview, background[opaStackCount - 1], renderedRects);
        curview = nullptr;
    } while (stackCount > 0);
    renderedRects.Clear();
}

void RootView::DrawInvalidMap(const Rect& buffRect)
{
    OptimizeInvalidMap();
    Rect rect;
    for (auto& viewEntry : invalidateMap_) {
        Vector<Rect>& viewRenderRect = viewEntry.second;
        for (uint16_t i = 0; i < viewRenderRect.Size(); i++) {
            rect.Intersect(viewRenderRect[i], buffRect);
            DrawTop(viewEntry.first, rect);
        }
    }
}
#endif

void RootView::AddInvalidateRect(Rect& rect, UIView* view)
{
    Rect commonRect(rect);
    if (IntersectScreenRect(commonRect)) {
#if LOCAL_RENDER
        Vector<Rect>& invalidRects = invalidateMap_[view];
        if (invalidRects.IsEmpty()) {
            invalidRects.PushBack(commonRect);
        } else {
            invalidRects[0].Join(invalidRects[0], commonRect);
        }
#else
        invalidRect_.Join(invalidRect_, commonRect);
        renderFlag_ = true;
#endif
    }
}

void RootView::AddInvalidateRectWithLock(Rect& rect, UIView* view)
{
#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_lock(&lock_);
#endif

    AddInvalidateRect(rect, view);

#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_unlock(&lock_);
#endif
}

void RootView::Measure()
{
#if LOCAL_RENDER
    if (!invalidateMap_.empty()) {
        MeasureView(childrenHead_);
    }
#else
    if (renderFlag_) {
        MeasureView(childrenHead_);
    }
#endif
}

void RootView::MeasureView(UIView* view)
{
    int16_t stackCount = 0;
    UIView* curView = view;
    while (stackCount >= 0) {
        while (curView != nullptr) {
            if (curView->IsVisible()) {
                curView->ReMeasure();
                if (curView->IsViewGroup() && stackCount < COMPONENT_NESTING_DEPTH) {
                    g_viewStack[stackCount++] = curView;
                    curView = static_cast<UIViewGroup*>(curView)->GetChildrenHead();
                    continue;
                }
            }
            curView = curView->GetNextSibling();
        }
        if (--stackCount >= 0) {
            curView = (g_viewStack[stackCount])->GetNextSibling();
        }
    }
}

void RootView::Render()
{
#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_lock(&lock_);
#endif

#if LOCAL_RENDER
    if (!invalidateMap_.empty()) {
        RenderManager::RenderRect(GetRect(), this);
        invalidateMap_.clear();
#else
    if (renderFlag_) {
        RenderManager::RenderRect(invalidRect_, this);
        invalidRect_ = {0, 0, 0, 0};
        renderFlag_ = false;
#endif

#if ENABLE_WINDOW
        if (boundWindow_) {
            boundWindow_->Flush();
            boundWindow_->Update();
        }
#endif
        ScreenDeviceProxy::GetInstance()->OnRenderFinish();
    }

#if defined __linux__ || defined __LITEOS__ || defined __APPLE__
    pthread_mutex_unlock(&lock_);
#endif
}

void RootView::DrawTop(UIView* view, const Rect& rect)
{
    if (view == nullptr) {
        return;
    }

    int16_t stackCount = 0;
    UIView* par = view->GetParent();
    if (par == nullptr) {
        par = view;
    }
    UIView* curView = view;
    UIView* transViewGroup = nullptr;
    Rect curViewRect;
    Rect mask = rect;
    Rect OrigRect;
    Rect RelativeRect;
    while (par != nullptr) {
        if (curView != nullptr) {
            if (curView->IsVisible()) {
                curViewRect = curView->GetMaskedRect();
                if (curViewRect.Intersect(curViewRect, mask) || enableAnimator_) {
                    if ((curView->GetViewType() != UI_IMAGE_VIEW) && (curView->GetViewType() != UI_TEXTURE_MAPPER) &&
                        !curView->IsTransInvalid() && !enableAnimator_) {
                        OrigRect = curView->GetOrigRect();
                        RelativeRect = curView->GetRelativeRect();
                        curView->GetTransformMap().SetInvalid(true);
                        curView->SetPosition(RelativeRect.GetX() - OrigRect.GetX(),
                                             RelativeRect.GetY() - OrigRect.GetY());
                        ScreenDeviceProxy::GetInstance()->EnableAnimatorBuffer(true);
                        ScreenDeviceProxy::GetInstance()->SetAnimatorRect(OrigRect);
                        ScreenDeviceProxy::GetInstance()->SetAnimatorTransMap(curView->GetTransformMap());
                        enableAnimator_ = true;
                    }
                    if (enableAnimator_) {
                        Rect invalidatedArea;
                        invalidatedArea.SetWidth(ScreenDeviceProxy::GetInstance()->GetScreenWidth());
                        invalidatedArea.SetHeight(ScreenDeviceProxy::GetInstance()->GetScreenHeight());
                        curView->OnDraw(invalidatedArea);
                    } else {
                        curView->OnDraw(curViewRect);
                    }

                    if ((curView->IsViewGroup()) && (stackCount < COMPONENT_NESTING_DEPTH)) {
                        if (enableAnimator_ && (transViewGroup == nullptr)) {
                            transViewGroup = curView;
                        }
                        par = curView;
                        g_viewStack[stackCount] = curView;
                        g_maskStack[stackCount] = mask;
                        stackCount++;
                        curView = static_cast<UIViewGroup*>(curView)->GetChildrenHead();
                        mask = par->GetContentRect();
                        mask.Intersect(mask, curViewRect);
                        continue;
                    }
                    curView->OnPostDraw(curViewRect);
                    if (enableAnimator_ && (transViewGroup == nullptr)) {
                        ScreenDeviceProxy::GetInstance()->EnableAnimatorBuffer(false);
                        ScreenDeviceProxy::GetInstance()->DrawAnimatorBuffer(mask);
                        curView->GetTransformMap().SetInvalid(false);
                        enableAnimator_ = false;
                        curView->SetPosition(RelativeRect.GetX(), RelativeRect.GetY());
                    }
                }
            }
            curView = curView->GetNextSibling();
            continue;
        }
        if (--stackCount >= 0) {
            curViewRect = par->GetMaskedRect();
            mask = g_maskStack[stackCount];
            if (curViewRect.Intersect(curViewRect, g_maskStack[stackCount])) {
                par->OnPostDraw(curViewRect);
            }
            if (enableAnimator_ && transViewGroup == g_viewStack[stackCount]) {
                ScreenDeviceProxy::GetInstance()->EnableAnimatorBuffer(false);
                ScreenDeviceProxy::GetInstance()->DrawAnimatorBuffer(mask);
                transViewGroup->GetTransformMap().SetInvalid(false);
                enableAnimator_ = false;
                transViewGroup->SetPosition(RelativeRect.GetX(), RelativeRect.GetY());
                transViewGroup = nullptr;
            }
            curView = g_viewStack[stackCount]->GetNextSibling();
            par = par->GetParent();
            continue;
        }
        stackCount = 0;
        curView = par->GetNextSibling();
        par = par->GetParent();
    }
}

UIView* RootView::GetTopUIView(const Rect& rect)
{
    int16_t stackCount = 0;
    UIView* currentView = this;
    UIView* topView = currentView;
    Rect copyRect(rect);
    while (stackCount >= 0) {
        while (currentView != nullptr) {
            if (currentView->GetOrigRect().IsContains(rect) && currentView->IsVisible()) {
                if (currentView->GetStyleConst().bgOpa_ == OPA_OPAQUE && currentView->OnPreDraw(copyRect) &&
                    currentView->GetOpaScale() == OPA_OPAQUE) {
                    topView = currentView;
                }
                if (currentView->IsViewGroup() && stackCount < COMPONENT_NESTING_DEPTH) {
                    g_viewStack[stackCount++] = currentView;
                    currentView = static_cast<UIViewGroup*>(currentView)->GetChildrenHead();
                    continue;
                }
            }
            currentView = currentView->GetNextSibling();
        }
        if (--stackCount >= 0) {
            currentView = (g_viewStack[stackCount])->GetNextSibling();
        }
    }
    return topView;
}

bool RootView::FindSubView(const UIView& parentView, const UIView* subView)
{
    const UIView* root = &parentView;
    if (root == subView) {
        return true;
    } else if (!root->IsViewGroup() || (subView == nullptr)) {
        return false;
    }

    UIView* currentView = static_cast<const UIViewGroup*>(root)->GetChildrenHead();
    const UIView* parent = root;
    int8_t deep = 1;
    while (deep > 0) {
        if (currentView == subView) {
            return true;
        }
        if (currentView == nullptr) {
            currentView = parent->GetNextSibling();
            parent = parent->GetParent();
            deep--;
        } else if (currentView->IsViewGroup()) {
            parent = currentView;
            currentView = static_cast<UIViewGroup*>(currentView)->GetChildrenHead();
            deep++;
        } else {
            currentView = currentView->GetNextSibling();
        }
    }
    return false;
}
} // namespace OHOS