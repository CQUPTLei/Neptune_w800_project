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

#ifndef GRAPHIC_LITE_FOCUS_MANAGER_H
#define GRAPHIC_LITE_FOCUS_MANAGER_H

#include "components/ui_view.h"

namespace OHOS {
#if ENABLE_MOTOR
/**
 * @brief 震动类型.
 *
 * @since 5.0
 * @version 3.0
 */
enum class MotorType {
    MOTOR_TYPE_ONE,
    MOTOR_TYPE_TWO,
};

/**
 * @brief 震动函数.
 *
 * @param type 震动类型.
 */
typedef void(*MotorFunc)(MotorType motorType);
#endif

class FocusManager {
public:
    static FocusManager* GetInstance()
    {
        static FocusManager instance;
        return &instance;
    }
    void ClearFocus()
    {
        view_ = nullptr;
    }

    UIView* GetFocusedView()
    {
        return view_;
    }

    void RequestFocus(UIView* view)
    {
        view_ = view;
    }

#if ENABLE_MOTOR
    void RegisterMotorFunc(MotorFunc motorFunc)
    {
        motorFunc_ = motorFunc;
    }

    MotorFunc GetMotorFunc()
    {
        return motorFunc_;
    }
#endif

private:
    FocusManager() : view_(nullptr) {}
    ~FocusManager() {}
    UIView* view_;
#if ENABLE_MOTOR
    MotorFunc motorFunc_ = nullptr;
#endif
};
} // namespace OHOS
#endif // GRAPHIC_LITE_FOCUS_MANAGER_H
