import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';
import withThemeName, { ThemeName } from '../Preferences/withThemeName';
import colors from '../../configs/colors';

type Props = {
  children: React.ReactNode;
  theme: ThemeName;
};

function ContentShell({ children, theme }: Props) {
  return (
    <div
      className={css(
        styles.container,
        theme === 'dark' ? styles.backgroundDark : styles.backgroundLight
      )}>
      {children}
    </div>
  );
}

export default withThemeName(ContentShell);

const styles = StyleSheet.create({
  container: {
    display: 'flex',
    flexDirection: 'column',
    height: '100%',
    width: '100%',
    overflow: 'hidden',
  },

  backgroundLight: {
    backgroundColor: colors.background.light,
    color: colors.text.light,
  },

  backgroundDark: {
    backgroundColor: colors.background.dark,
    color: colors.text.dark,
  },
});
