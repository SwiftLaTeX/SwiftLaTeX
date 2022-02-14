import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';
import colors from '../configs/colors';
import { EngineVersion, EngineVersions } from '../types';
import usePreferences from './Preferences/usePreferences';

type Props = {
    sdkVersion: EngineVersion;
    onChange: (sdkVersion: EngineVersion) => void;
};

export default function EngineVersionSwitcher({ sdkVersion, onChange }: Props) {
    const [prefs] = usePreferences();

    return (
        <div className={css(styles.container)}>
            <span className={css(styles.label)}>Typesetting Engine: </span>
            <span className={css(styles.switcher)}>
                <select
                    value={sdkVersion}
                    onChange={(e) => onChange(e.target.value as any)}
                    className={css(
                        styles.select,
                        prefs.theme === 'dark' ? styles.dark : styles.light
                    )}>
                    {Object.keys(EngineVersions).map((v) => (
                        <option key={v} value={v}>
                            {v}
                        </option>
                    ))}
                </select>
            </span>
        </div>
    );
}

const styles = StyleSheet.create({
    container: {
        display: 'flex',
        alignItems: 'center',
        whiteSpace: 'nowrap',
        marginRight: '.5em',
    },
    switcher: {
        position: 'relative',
        display: 'inline-block',

        ':after': {
            content: '"▼"',
            position: 'absolute',
            fontSize: '0.6em',
            right: '2em',
            top: '1em',
            pointerEvents: 'none',
        },
    },
    label: {
        flex: 1,
        margin: '0 .5em',

        '@media (max-width: 480px)': {
            display: 'none',
        },
    },
    select: {
        appearance: 'none',
        backgroundColor: 'transparent',
        padding: '0 2em 0 1em',
        borderRadius: 12,
        outline: 0,
    },
    light: {
        border: `1px solid ${colors.border}`,
    },
    dark: {
        border: '1px solid rgba(255, 255, 255, .2)',
    },
});
