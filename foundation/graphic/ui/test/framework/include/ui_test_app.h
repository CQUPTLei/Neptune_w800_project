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

#ifndef UI_TEST_APP_LIST_H
#define UI_TEST_APP_LIST_H

#include "components/root_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_list.h"
#include "test_case_list_adapter.h"

namespace OHOS {
class UITestApp {
public:
    static UITestApp* GetInstance()
    {
        static UITestApp instance;
        return &instance;
    }
    void Start();
    void Init();

private:
    UITestApp() {}
    ~UITestApp();

    UITestApp(const UITestApp&) = delete;
    UITestApp& operator=(const UITestApp&) = delete;
    UITestApp(UITestApp&&) = delete;
    UITestApp& operator=(UITestApp&&) = delete;

    RootView* rootView_ = nullptr;
    UIList* mainList_ = nullptr;
    TestCaseListAdapter* adapter_ = nullptr;
    UILabelButton* backBtn_ = nullptr;
    UILabel* testCaseLabel_ = nullptr;
    UILabel* testLabel_ = nullptr;
};
} // namespace OHOS
#endif