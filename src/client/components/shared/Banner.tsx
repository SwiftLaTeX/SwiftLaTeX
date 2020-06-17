import * as React from 'react';
import classnames from 'classnames';
import { StyleSheet, css } from 'aphrodite';

import colors from '../../configs/colors';

type Props = {
  type?: 'info' | 'success' | 'error';
  visible: boolean;
  children?: React.ReactNode;
  className?: string;
};

type State = {
  rendered: boolean;
};

export default class Banner extends React.PureComponent<Props, State> {
  static getDerivedStateFromProps(props: Props) {
    if (props.visible) {
      return {
        rendered: true,
      };
    }

    return null;
  }

  state = {
    rendered: this.props.visible,
  };

  componentDidUpdate(prevProps: Props) {
    if (this.props.visible !== prevProps.visible) {
      clearTimeout(this._timer);

      if (!this.props.visible) {
        this._timer = setTimeout(() => this.setState({ rendered: false }), 300);
      }
    }
  }

  _timer: any;

  render() {
    if (!this.state.rendered) {
      return null;
    }

    return (
      <div className={css(styles.container)}>
        <div
          className={classnames(
            this.props.className,
            css(
              styles.banner,
              this.props.visible ? styles.visible : styles.hidden,
              this.props.type === 'success' && styles.success,
              this.props.type === 'error' && styles.error
            )
          )}>
          {this.props.children}
        </div>
      </div>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    position: 'fixed',
    top: 0,
    left: 0,
    right: 0,
    padding: '0 10px',
    zIndex: 999,
    textAlign: 'center',
    pointerEvents: 'none',
  },
  banner: {
    display: 'inline-block',
    margin: 'auto',
    padding: '1em',
    maxWidth: '100%',
    backgroundColor: colors.primary,
    color: '#fff',
    borderRadius: '0 0 4px 4px',
    transitionDuration: '200ms',
    pointerEvents: 'all',
  },
  visible: {
    transform: 'translateY(0)',
    pointerEvents: 'auto',
  },
  hidden: {
    transform: 'translateY(-100%)',
    pointerEvents: 'none',
  },
  success: {
    backgroundColor: colors.success,
  },
  error: {
    backgroundColor: colors.error,
  },
});
