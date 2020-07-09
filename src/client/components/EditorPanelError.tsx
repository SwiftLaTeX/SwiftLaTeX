import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';
import { Annotation } from '../types';
import colors from '../configs/colors';

type Props = {
    annotations: Annotation[];
};

export default function EditorPanelError({ annotations }: Props) {
    return (
        <div>
            <div className={css(styles.container)}>
                <div className={css(styles.line)}>
                    {annotations.map((annotation, i) => (
                        <div key={i} className={css(annotation.severity > 3 ? styles.error : styles.warning, styles.perLine)}>
                            {i}: {annotation.source}-({annotation.startLineNumber}:
                            {annotation.startColumn}) {annotation.message.split('\n')[0]}

                        </div>

                    ))}
                </div>
            </div>
        </div>
    );
}

const styles = StyleSheet.create({
    container: {
        display: 'flex',
        flexDirection: 'row',
        flexWrap: 'nowrap',
        fontFamily: 'var(--font-monospace)',
        fontSize: 13,
        minHeight: 16,
        margin: '4px 0',
    },
    line: {
        display: 'flex',
        flexDirection: 'column',
        flexWrap: 'wrap',
    },
    perLine: {
        flex: 1
    },
    error: {
        color: colors.error,
    },
    warning: {
        color: colors.warning,
    },
});
