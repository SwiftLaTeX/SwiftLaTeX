import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';
import mapKeys from 'lodash/mapKeys';
import usePreferences from './Preferences/usePreferences';

type Colors = typeof light;

const common = {
    primary: '#4630eb',
    secondary: '#5844ed',

    error: '#f44336',
    warning: '#ff9800',
    success: '#4caf50',

    border: 'rgba(0, 0, 0, .08)',
};

export const light = {
    ...common,

    accent: '#000',
    'accent-text': '#fff',

    text: '#111',
    background: '#fafafa',
    content: '#fff',

    // Editor colors
    'editor-background': '#fafafa',
    'editor-text': '#111',
    'editor-border': 'transparent',
};

export const dark: Colors = {
    ...common,

    accent: '#fff',
    'accent-text': '#000',

    text: '#D3D7CE',
    background: '#212733',
    content: '#242B38',

    // Editor colors
    'editor-background': '#272D38',
    'editor-text': '#D9D7CE',
    'editor-border': '#343D4A',
};

export const c = (name: keyof typeof light) => `var(--color-${name})`;

type Props = {
    children: React.ReactNode;
};

export default function ColorsProvider({ children }: Props) {
    const [prefs] = usePreferences();

    return (
        <div className={css(styles.container, prefs.theme === 'dark' ? styles.dark : styles.light)}>
            {children}
        </div>
    );
}

const styles = StyleSheet.create({
    container: {
        height: '100%',
        width: '100%',
    },
    light: mapKeys(light, (_, key) => `--color-${key}`),
    dark: mapKeys(dark, (_, key) => `--color-${key}`),
});
