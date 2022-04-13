/* GENERATED SOURCE. DO NOT MODIFY. */
// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html#License
package ohos.global.icu.impl.number;

import ohos.global.icu.number.Scale;

/**
 * Wraps a {@link Scale} for use in the number formatting pipeline.
 * @hide exposed on OHOS
 */
public class MultiplierFormatHandler implements MicroPropsGenerator {
    final Scale multiplier;
    final MicroPropsGenerator parent;

    public MultiplierFormatHandler(Scale multiplier, MicroPropsGenerator parent) {
        this.multiplier = multiplier;
        this.parent = parent;
    }

    @Override
    public MicroProps processQuantity(DecimalQuantity quantity) {
        MicroProps micros = parent.processQuantity(quantity);
        multiplier.applyTo(quantity);
        return micros;
    }
}
