/* GENERATED SOURCE. DO NOT MODIFY. */
// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html#License
/*
 *******************************************************************************
 * Copyright (C) 2013-2014, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package ohos.global.icu.text;

import java.text.FieldPosition;
import java.text.Format.Field;

/**
 * Adds the ability to get the decimal digits
 * @deprecated This API is ICU internal only.
 * @hide exposed on OHOS
 * @hide draft / provisional / internal are hidden on OHOS
 */
@Deprecated
public class UFieldPosition extends FieldPosition {
    private int countVisibleFractionDigits = -1;
    private long fractionDigits = 0;

    /**
     * @deprecated This API is ICU internal only.
     * @hide draft / provisional / internal are hidden on OHOS
     */
    @Deprecated
    public UFieldPosition() {
        super(-1);
    }

    /**
     * @deprecated This API is ICU internal only.
     * @hide draft / provisional / internal are hidden on OHOS
     */
    @Deprecated
    public UFieldPosition(int field) {
        super(field);
    }

    /**
     * @deprecated This API is ICU internal only.
     * @hide draft / provisional / internal are hidden on OHOS
     */
    @Deprecated
    public UFieldPosition(Field attribute, int fieldID) {
        super(attribute, fieldID);
    }

    /**
     * @deprecated This API is ICU internal only.
     * @hide draft / provisional / internal are hidden on OHOS
     */
    @Deprecated
    public UFieldPosition(Field attribute) {
        super(attribute);
    }

    /**
     * @deprecated This API is ICU internal only.
     * @hide draft / provisional / internal are hidden on OHOS
     */
    @Deprecated
    public void setFractionDigits(int countVisibleFractionDigits, long fractionDigits ) {
        this.countVisibleFractionDigits = countVisibleFractionDigits;
        this.fractionDigits = fractionDigits;
    }

    /**
     * @deprecated This API is ICU internal only.
     * @hide draft / provisional / internal are hidden on OHOS
     */
    @Deprecated
    public int getCountVisibleFractionDigits() {
        return countVisibleFractionDigits;
    }

    /**
     * @deprecated This API is ICU internal only.
     * @hide draft / provisional / internal are hidden on OHOS
     */
    @Deprecated
    public long getFractionDigits() {
        return fractionDigits;
    }
}
