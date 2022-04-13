/* GENERATED SOURCE. DO NOT MODIFY. */
// © 2018 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html#License
package ohos.global.icu.impl.number;

import ohos.global.icu.impl.StandardPlural;
import ohos.global.icu.impl.number.Modifier.Signum;

/**
 * This is *not* a modifier; rather, it is an object that can return modifiers
 * based on given parameters.
 *
 * @author sffc
 * @hide exposed on OHOS
 */
public interface ModifierStore {
    /**
     * Returns a Modifier with the given parameters (best-effort).
     */
    Modifier getModifier(Signum signum, StandardPlural plural);
}
