/* GENERATED SOURCE. DO NOT MODIFY. */
// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html#License
package ohos.global.icu.impl.number.parse;

import static ohos.global.icu.impl.number.parse.ParsingUtils.safeContains;

import ohos.global.icu.impl.StaticUnicodeSets;
import ohos.global.icu.impl.StringSegment;
import ohos.global.icu.text.DecimalFormatSymbols;

/**
 * @author sffc
 * @hide exposed on OHOS
 *
 */
public class MinusSignMatcher extends SymbolMatcher {

    private static final MinusSignMatcher DEFAULT = new MinusSignMatcher(false);
    private static final MinusSignMatcher DEFAULT_ALLOW_TRAILING = new MinusSignMatcher(true);

    public static MinusSignMatcher getInstance(DecimalFormatSymbols symbols, boolean allowTrailing) {
        String symbolString = symbols.getMinusSignString();
        if (safeContains(DEFAULT.uniSet, symbolString)) {
            return allowTrailing ? DEFAULT_ALLOW_TRAILING : DEFAULT;
        } else {
            return new MinusSignMatcher(symbolString, allowTrailing);
        }
    }

    private final boolean allowTrailing;

    private MinusSignMatcher(String symbolString, boolean allowTrailing) {
        super(symbolString, DEFAULT.uniSet);
        this.allowTrailing = allowTrailing;
    }

    private MinusSignMatcher(boolean allowTrailing) {
        super(StaticUnicodeSets.Key.MINUS_SIGN);
        this.allowTrailing = allowTrailing;
    }

    @Override
    protected boolean isDisabled(ParsedNumber result) {
        return !allowTrailing && result.seenNumber();
    }

    @Override
    protected void accept(StringSegment segment, ParsedNumber result) {
        result.flags |= ParsedNumber.FLAG_NEGATIVE;
        result.setCharsConsumed(segment);
    }

    @Override
    public String toString() {
        return "<MinusSignMatcher>";
    }
}
