import findKey from 'lodash/findKey';
import { KeyMap } from './KeybindingsManager';

type Props = {
    combo: number[];
};

type KeyName = keyof typeof KeyMap;

const isMac = 'navigator' in global && /Mac/i.test(navigator.platform);

const KeyLabels: Partial<{ [key in KeyName]: string }> = {
    Cmd: '⌘',
    Delete: '⌫',
    Enter: '↩',
    Shift: '⇧',
    Ctrl: isMac ? '⌃' : 'Ctrl',
    Alt: isMac ? '⌥' : 'Alt',
    Backslash: '\\',
    Tilde: '`',
};

export default function ShortcutLabel({ combo }: Props): any {
    return combo
        .map((code) => {
            const name = findKey(KeyMap, (c) => c === code);

            // @ts-ignore
            if (name && KeyLabels[name]) {
                // @ts-ignore
                return KeyLabels[name];
            } else {
                return name;
            }
        })
        .join(isMac ? '' : '+');
}
