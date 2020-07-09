import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';
import withThemeName, { ThemeName } from './Preferences/withThemeName';
import { EngineVersion } from '../types';

type Props = {
    previewRef: React.MutableRefObject<Window | null>;
    theme: ThemeName;
    engine: EngineVersion;
    onPreviewClick: (path: string, line: number, column: number) => void;
};

function WebFrame({ previewRef, theme, onPreviewClick, engine }: Props) {
    const url = engine === 'XeLaTeX' ? '/dviviewer.html' : '/pdfviewer.html';

    React.useEffect(() => {
        const handler = (ev: any) => {
            if (ev.data.cmd === 'setCursor') {
                onPreviewClick(ev.data.path, ev.data.line, ev.data.column);
            }
        };
        window.addEventListener('message', handler);
        // clean up
        return () => window.removeEventListener('message', handler);
    }, []); // empty array => run only once)

    return (
        <div className={css(styles.pane)}>
            <iframe
                ref={(c) => (previewRef.current = c ? c.contentWindow : null)}
                src={url}
                className={css(
                    styles.frame,
                    theme === 'dark' ? styles.frameDark : styles.frameLight
                )}
            />
        </div>
    );
}

export default withThemeName(WebFrame);

const styles = StyleSheet.create({
    pane: {
        position: 'relative',
        display: 'flex',
        flex: 1,
        height: '100%',
    },
    frame: {
        position: 'relative',
        width: '100%',
        height: '100%',
        border: 0,
    },
    frameLight: {
        backgroundColor: '#fefefe',
    },
    frameDark: {
        backgroundColor: '#ffffff',
    },
    spinnerContainer: {
        position: 'absolute',
        top: 0,
        left: 0,
        right: 0,
        bottom: 0,
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'center',
    },
});
