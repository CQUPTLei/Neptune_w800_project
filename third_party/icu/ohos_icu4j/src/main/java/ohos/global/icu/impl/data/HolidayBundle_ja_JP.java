/* GENERATED SOURCE. DO NOT MODIFY. */
// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html#License
/*
 *******************************************************************************
 * Copyright (C) 1996-2010, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

package ohos.global.icu.impl.data;

import java.util.Calendar;
import java.util.ListResourceBundle;

import ohos.global.icu.util.Holiday;
import ohos.global.icu.util.SimpleHoliday;

/**
 * @hide exposed on OHOS
 */
public class HolidayBundle_ja_JP extends ListResourceBundle {
    static private final Holiday[] fHolidays = {
        new SimpleHoliday(Calendar.FEBRUARY,  11,  0,    "National Foundation Day"),
    };
    static private final Object[][] fContents = {
        {   "holidays",         fHolidays   },
    };
    @Override
    public synchronized Object[][] getContents() { return fContents; }
}
