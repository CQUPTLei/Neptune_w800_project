/* GENERATED SOURCE. DO NOT MODIFY. */
// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html#License
package ohos.global.icu.impl.number.parse;

import ohos.global.icu.impl.StringSegment;

/**
 * A Matcher used only for post-process validation, not for consuming characters at runtime.
 * @hide exposed on OHOS
 */
public abstract class ValidationMatcher implements NumberParseMatcher {

    @Override
    public boolean match(StringSegment segment, ParsedNumber result) {
        return false;
    }

    @Override
    public boolean smokeTest(StringSegment segment) {
        return false;
    }

}
