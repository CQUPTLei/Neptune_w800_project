/* GENERATED SOURCE. DO NOT MODIFY. */
// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html#License
package ohos.global.icu.impl.number.parse;

import ohos.global.icu.impl.StringSegment;

/**
 * Matches a single code point, performing no other logic.
 *
 * @author sffc
 * @hide exposed on OHOS
 */
public class CodePointMatcher implements NumberParseMatcher {

    private final int cp;

    public static CodePointMatcher getInstance(int cp) {
        // TODO: Cache certain popular instances?
        return new CodePointMatcher(cp);
    }

    private CodePointMatcher(int cp) {
        this.cp = cp;
    }

    @Override
    public boolean match(StringSegment segment, ParsedNumber result) {
        if (segment.startsWith(cp)) {
            segment.adjustOffsetByCodePoint();
            result.setCharsConsumed(segment);
        }
        return false;
    }

    @Override
    public boolean smokeTest(StringSegment segment) {
        return segment.startsWith(cp);
    }

    @Override
    public void postProcess(ParsedNumber result) {
        // No-op
    }

    @Override
    public String toString() {
        return "<CodePointMatcher U+" + Integer.toHexString(cp) + ">";
    }

}
