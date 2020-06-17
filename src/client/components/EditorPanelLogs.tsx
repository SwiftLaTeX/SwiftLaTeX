import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';

type Props = {
  deviceLogs: string;
};

export default function EditorPanelLogs({ deviceLogs }: Props) {
  return (
    <div>
      <div className={css(styles.container)}>
        <div className={css(styles.line)}>
          <pre>{deviceLogs}</pre>
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
    flexDirection: 'row',
    flexWrap: 'wrap',
  },
});
