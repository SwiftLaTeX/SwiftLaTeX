import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';

type Props = {
  children: React.ReactNode;
};

export default function ToolbarTitleShell({ children }: Props) {
  return <div className={css(styles.left)}>{children}</div>;
}

const styles = StyleSheet.create({
  left: {
    display: 'flex',
    flexDirection: 'row',
    alignItems: 'center',
    minWidth: 0,
    flex: 1,
  },
});
