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

import ohos.global.icu.util.EasterHoliday;
import ohos.global.icu.util.Holiday;
import ohos.global.icu.util.SimpleHoliday;

/**
 * @hide exposed on OHOS
 */
public class HolidayBundle_el_GR extends ListResourceBundle {
    static private final Holiday[] fHolidays = {
        SimpleHoliday.NEW_YEARS_DAY,
        SimpleHoliday.EPIPHANY,

        new SimpleHoliday(Calendar.MARCH,     25,  0,    "Independence Day"),

        SimpleHoliday.MAY_DAY,
        SimpleHoliday.ASSUMPTION,

        new SimpleHoliday(Calendar.OCTOBER,   28,  0,    "Ochi Day"),

        SimpleHoliday.CHRISTMAS,
        SimpleHoliday.BOXING_DAY,

        // Easter and related holidays in the Orthodox calendar
        new EasterHoliday(-2,   true, "Good Friday"),
        new EasterHoliday( 0,   true, "Easter Sunday"),
        new EasterHoliday( 1,   true, "Easter Monday"),
        new EasterHoliday(50,   true, "Whit Monday"),
    };
    static private final Object[][] fContents = {
        {   "holidays",         fHolidays   },
    };
    @Override
    public synchronized Object[][] getContents() { return fContents; }
}
