import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';
import withThemeName, { ThemeName } from '../Preferences/withThemeName';
import colors from '../../configs/colors';

type Props = {
    children: React.ReactNode;
    theme: ThemeName;
};

function ToolbarShell({ children, theme }: Props) {
    return (
        <div
            className={css(
                styles.toolbar,
                theme === 'dark' ? styles.toolbarDark : styles.toolbarLight
            )}>
            {children}
        </div>
    );
}

export default withThemeName(ToolbarShell);

const styles = StyleSheet.create({
    toolbar: {
        display: 'flex',
        flexDirection: 'row',
        alignItems: 'center',
        justifyContent: 'space-between',
        borderBottom: `1px solid ${colors.border}`,
        height: 60,
    },

    toolbarLight: {
        backgroundColor: colors.content.light,
    },

    toolbarDark: {
        backgroundColor: colors.content.dark,
    },
});
