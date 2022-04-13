/* GENERATED SOURCE. DO NOT MODIFY. */
// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html#License
package ohos.global.icu.impl.number.parse;

import ohos.global.icu.impl.StringSegment;
import ohos.global.icu.text.UnicodeSet;

/**
 * @author sffc
 * @hide exposed on OHOS
 *
 */
public class PaddingMatcher extends SymbolMatcher implements NumberParseMatcher.Flexible {

    public static PaddingMatcher getInstance(String padString) {
        return new PaddingMatcher(padString);
    }

    private PaddingMatcher(String symbolString) {
        super(symbolString, UnicodeSet.EMPTY);
    }

    @Override
    protected boolean isDisabled(ParsedNumber result) {
        return false;
    }

    @Override
    protected void accept(StringSegment segment, ParsedNumber result) {
        // No-op
    }

    @Override
    public String toString() {
        return "<PaddingMatcher>";
    }
}
