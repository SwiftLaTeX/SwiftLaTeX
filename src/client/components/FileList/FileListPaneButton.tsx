import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';

type Props = {
  innerRef?: React.Ref<HTMLButtonElement>;
  title?: string;
  onClick: (event: React.MouseEvent<HTMLButtonElement>) => void;
  children?: React.ReactNode;
};

class FileListPaneButton extends React.PureComponent<Props> {
  render() {
    const { innerRef, ...rest } = this.props;

    return (
      <button {...rest} className={css(styles.button)} ref={innerRef}>
        <svg className={css(styles.icon)} viewBox="0 0 16 16">
          {this.props.children}
        </svg>
      </button>
    );
  }
}

export default React.forwardRef((props: Props, ref: React.Ref<HTMLButtonElement>) => (
  <FileListPaneButton {...props} innerRef={ref} />
));

const styles = StyleSheet.create({
  button: {
    appearance: 'none',
    background: 'transparent',
    border: 0,
    margin: 0,
    outline: 0,
  },
  icon: {
    fill: 'currentColor',
    height: 16,
    width: 16,
    verticalAlign: -3,
  },
});
