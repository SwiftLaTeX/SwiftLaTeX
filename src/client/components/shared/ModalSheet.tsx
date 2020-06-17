import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';
import classnames from 'classnames';
import Modal from './Modal';
import withThemeName, { ThemeName } from '../Preferences/withThemeName';
import colors from '../../configs/colors';

export type ModalSheetProps = {
  visible: boolean;
  onDismiss: () => void;
  children: React.ReactNode;
  className?: string;
};

type Props = ModalSheetProps & {
  theme: ThemeName;
};

function ModalSheet(props: Props) {
  return (
    <Modal visible={props.visible} onDismiss={props.onDismiss}>
      <div
        className={classnames(
          css(styles.modal, props.theme === 'dark' ? styles.contentDark : styles.contentLight),
          props.className
        )}>
        {props.onDismiss ? (
          <button
            className={css(styles.close)}
            onClick={props.onDismiss}
            data-test-id="modal-close">
            ✕
          </button>
        ) : null}
        {props.children}
      </div>
    </Modal>
  );
}

export default withThemeName(ModalSheet);

const styles = StyleSheet.create({
  modal: {
    display: 'flex',
    flexDirection: 'column',
    position: 'relative',
    textAlign: 'center',
    borderRadius: 4,
    boxShadow: '0 1px 4px rgba(36, 44, 58, 0.3)',
  },
  close: {
    appearance: 'none',
    borderRadius: '1em',
    outline: 0,
    padding: 0,
    position: 'absolute',
    right: '-1em',
    top: '-1em',
    width: '2em',
    height: '2em',
    background: colors.background.dark,
    border: `2px solid ${colors.background.light}`,
    boxShadow: '0 1.5px 3px rgba(0, 0, 0, .16)',
    color: 'white',
    fontSize: '1em',
    fontWeight: 'bold',
    textAlign: 'center',
  },
  contentLight: {
    backgroundColor: colors.content.light,
    color: colors.text.light,
  },
  contentDark: {
    backgroundColor: colors.content.dark,
    color: colors.text.dark,
    border: `1px solid ${colors.border}`,
  },
});
