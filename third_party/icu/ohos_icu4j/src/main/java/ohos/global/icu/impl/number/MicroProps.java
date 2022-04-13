/* GENERATED SOURCE. DO NOT MODIFY. */
// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html#License
package ohos.global.icu.impl.number;

import ohos.global.icu.number.IntegerWidth;
import ohos.global.icu.number.NumberFormatter.DecimalSeparatorDisplay;
import ohos.global.icu.number.NumberFormatter.SignDisplay;
import ohos.global.icu.number.Precision;
import ohos.global.icu.text.DecimalFormatSymbols;

/**
 * @hide exposed on OHOS
 */
public class MicroProps implements Cloneable, MicroPropsGenerator {
    // Populated globally:
    public SignDisplay sign;
    public DecimalFormatSymbols symbols;
    public String nsName;
    public Padder padding;
    public DecimalSeparatorDisplay decimal;
    public IntegerWidth integerWidth;

    // Populated by notation/unit:
    public Modifier modOuter;
    public Modifier modMiddle;
    public Modifier modInner;
    public Precision rounder;
    public Grouper grouping;
    public boolean useCurrency;

    // Internal fields:
    private final boolean immutable;
    private volatile boolean exhausted;

    /**
     * @param immutable
     *            Whether this MicroProps should behave as an immutable after construction with respect
     *            to the quantity chain.
     */
    public MicroProps(boolean immutable) {
        this.immutable = immutable;
    }

    @Override
    public MicroProps processQuantity(DecimalQuantity quantity) {
        if (immutable) {
            return (MicroProps) this.clone();
        } else if (exhausted) {
            // Safety check
            throw new AssertionError("Cannot re-use a mutable MicroProps in the quantity chain");
        } else {
            exhausted = true;
            return this;
        }
    }

    @Override
    public Object clone() {
        try {
            return super.clone();
        } catch (CloneNotSupportedException e) {
            throw new AssertionError(e);
        }
    }
}
