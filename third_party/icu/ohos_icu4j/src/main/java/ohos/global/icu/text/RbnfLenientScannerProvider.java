/* GENERATED SOURCE. DO NOT MODIFY. */
// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html#License
/*
 *******************************************************************************
 * Copyright (C) 2009-2014, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

package ohos.global.icu.text;

import ohos.global.icu.util.ULocale;

/**
 * A provider for an RbnfLenientScanner.
 *
 * @deprecated ICU 54
 * @hide exposed on OHOS
 */
@Deprecated
public interface RbnfLenientScannerProvider {
    /**
     * Returns a scanner appropriate for the given locale, with optional extra data.
     * in the form of collation rules.
     *
     * @param locale the locale to provide the default lenient rules.
     * @param extras extra collation rules
     * @return the lenient scanner, or null
     * @deprecated ICU 54
     */
    @Deprecated
    RbnfLenientScanner get(ULocale locale, String extras);
}