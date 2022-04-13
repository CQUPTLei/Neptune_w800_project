/* GENERATED SOURCE. DO NOT MODIFY. */
// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html#License
/*
 *******************************************************************************
 * Copyright (C) 2011, International Business Machines Corporation and         *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */
package ohos.global.icu.impl;

import ohos.global.icu.text.TimeZoneNames;
import ohos.global.icu.text.TimeZoneNames.Factory;
import ohos.global.icu.util.ULocale;

/**
 * The implementation class of <code>TimeZoneNames.Factory</code>
 * @hide exposed on OHOS
 */
public class TimeZoneNamesFactoryImpl extends Factory {

    /* (non-Javadoc)
     * @see ohos.global.icu.text.TimeZoneNames.Factory#getTimeZoneNames(ohos.global.icu.util.ULocale)
     */
    @Override
    public TimeZoneNames getTimeZoneNames(ULocale locale) {
        return new TimeZoneNamesImpl(locale);
    }

}
