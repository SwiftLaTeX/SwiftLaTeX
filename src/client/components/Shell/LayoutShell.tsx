import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';

type Props = {
    children: React.ReactNode;
};

export default function LayoutShell({ children }: Props) {
    return <div className={css(styles.layout)}>{children}</div>;
}

const styles = StyleSheet.create({
    layout: {
        display: 'flex',
        flex: 1,
        flexDirection: 'row',
        position: 'relative',
        height: '100%',
        // Without this firefox doesn't shrink content
        minHeight: 0,
        minWidth: 0,
    },
});
