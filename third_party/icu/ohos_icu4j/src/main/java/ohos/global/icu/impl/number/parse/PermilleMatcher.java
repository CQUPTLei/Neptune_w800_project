/* GENERATED SOURCE. DO NOT MODIFY. */
// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html#License
package ohos.global.icu.impl.number.parse;

import ohos.global.icu.impl.StaticUnicodeSets;
import ohos.global.icu.impl.StringSegment;
import ohos.global.icu.text.DecimalFormatSymbols;

/**
 * @author sffc
 * @hide exposed on OHOS
 *
 */
public class PermilleMatcher extends SymbolMatcher {

    private static final PermilleMatcher DEFAULT = new PermilleMatcher();

    public static PermilleMatcher getInstance(DecimalFormatSymbols symbols) {
        String symbolString = symbols.getPerMillString();
        if (DEFAULT.uniSet.contains(symbolString)) {
            return DEFAULT;
        } else {
            return new PermilleMatcher(symbolString);
        }
    }

    private PermilleMatcher(String symbolString) {
        super(symbolString, DEFAULT.uniSet);
    }

    private PermilleMatcher() {
        super(StaticUnicodeSets.Key.PERMILLE_SIGN);
    }

    @Override
    protected boolean isDisabled(ParsedNumber result) {
        return 0 != (result.flags & ParsedNumber.FLAG_PERMILLE);
    }

    @Override
    protected void accept(StringSegment segment, ParsedNumber result) {
        result.flags |= ParsedNumber.FLAG_PERMILLE;
        result.setCharsConsumed(segment);
    }

    @Override
    public String toString() {
        return "<PermilleMatcher>";
    }
}
