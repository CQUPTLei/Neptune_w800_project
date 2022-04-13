/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

package ohos.junit.distribute;

/**
 * device enum
 */
public enum DeviceEnum {
    /**
     * first agent device
     */
    DEVICE_FIRST(1),

    /**
     * second agent device
     */
    DEVICE_SECOND(2),

    /**
     * third agent device
     */
    DEVICE_THIRD(3),

    /**
     * fourth agent device
     */
    DEVICE_FOURTH(4),

    /**
     * fifth agent device
     */
    DEVICE_FIFTH(5),

    /**
     * sixth agent device
     */
    DEVICE_SIXTH(6),

    /**
     * seventh agent device
     */
    DEVICE_SEVENTH(7),

    /**
     * eighth agent device
     */
    DEVICE_EIGHTH(8),

    /**
     * ninth agent device
     */
    DEVICE_NINTH(9);

    private final int value;

    DeviceEnum(int value) {
        this.value = value;
    }

    public int getValue() {
        return this.value;
    }
}
