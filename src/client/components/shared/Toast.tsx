import * as React from 'react';
import ReactDOM from 'react-dom';
import { StyleSheet, css } from 'aphrodite';
import withThemeName, { ThemeName } from '../Preferences/withThemeName';
import colors from '../../configs/colors';

type Props = {
  label: React.ReactNode;
  timeout?: number;
  persistent?: boolean;
  type?: 'success' | 'error';
  theme: ThemeName;
  actions: Array<{ label: string; action?: () => void; primary?: boolean }>;
  onDismiss?: () => void;
};

type State = {
  dismissing: boolean;
};

const TOAST_GROUP_ID = '__toast_group__container';

class Toast extends React.Component<Props, State> {
  static defaultProps = {
    timeout: 5000,
  };

  state = {
    dismissing: false,
  };

  componentDidMount() {
    let group = document.getElementById(TOAST_GROUP_ID);

    if (!group) {
      group = document.createElement('div');
      group.id = TOAST_GROUP_ID;

      Object.assign(group.style, {
        position: 'fixed',
        bottom: '3em',
        left: '1em',
        zIndex: '999',
      });

      document.body.appendChild(group);
    }

    group.appendChild(this._container);

    this.props.persistent || this._scheduleDismiss();
  }

  componentWillUnmount() {
    const group = document.getElementById(TOAST_GROUP_ID);

    if (group) {
      group.removeChild(this._container);
    }

    this._cancelDismiss();
  }

  _container = document.createElement('div');
  _timer: any;

  _scheduleDismiss = () => {
    this._timer = setTimeout(this._handleDismiss, this.props.timeout);
  };

  _cancelDismiss = () => {
    this.setState({ dismissing: false });
    clearTimeout(this._timer);
  };

  _handleDismiss = () => {
    this.setState({ dismissing: true });
    this._timer = setTimeout(() => {
      this.props.onDismiss && this.props.onDismiss();
    }, 400);
  };

  render() {
    const props = this.props.persistent
      ? {}
      : {
          onMouseEnter: this._cancelDismiss,
          onMouseLeave: this._scheduleDismiss,
        };

    return ReactDOM.createPortal(
      <div
        {...props}
        className={css(
          styles.toast,
          this.props.type
            ? this.props.type === 'error'
              ? styles.error
              : styles.success
            : this.props.theme === 'dark'
            ? styles.dark
            : styles.light,
          this.state.dismissing ? styles.dismissing : styles.appearing
        )}>
        <div className={css(styles.label)}>{this.props.label}</div>
        {this.props.actions.map(action => (
          <button
            key={action.label}
            className={css(
              styles.button,
              this.props.theme !== 'dark' && !this.props.type ? styles.buttonLight : null
            )}
            onClick={action.action || this._handleDismiss}>
            {action.label}
          </button>
        ))}
      </div>,
      this._container
    );
  }
}

export default withThemeName(Toast);

const fadeIn = {
  from: { opacity: 0 },
  to: { opacity: 1 },
};

const fadeOut = {
  from: { opacity: 1 },
  to: { opacity: 0 },
};

const styles = StyleSheet.create({
  toast: {
    display: 'flex',
    margin: '1em',
    padding: '0 .75em',
    borderRadius: 3,
    border: `1px solid ${colors.border}`,
    boxShadow: '0 4px 16px rgba(0, 1, 31, 0.2)',
    minWidth: '27em',
    whiteSpace: 'nowrap',
  },
  light: {
    backgroundColor: colors.background.light,
    color: colors.text.light,
  },
  dark: {
    backgroundColor: '#434852',
    color: '#fff',
  },
  success: {
    backgroundColor: colors.success,
    color: '#fff',
  },
  error: {
    backgroundColor: colors.error,
    color: '#fff',
  },
  appearing: {
    animationName: fadeIn,
    animationDuration: '250ms',
    opacity: 1,
  },
  dismissing: {
    animationName: fadeOut,
    animationDuration: '400ms',
    opacity: 0,
  },
  label: {
    flex: 1,
    minWidth: 0,
    overflow: 'hidden',
    textOverflow: 'ellipsis',
    padding: '.75em',
  },
  button: {
    appearance: 'none',
    background: 'transparent',
    border: '0',
    textTransform: 'uppercase',
    fontSize: '.9em',
    fontWeight: 'bold',
    padding: '1em',
    outline: 0,

    ':hover': {
      backgroundColor: 'rgba(0, 0, 0, .08)',
    },

    ':active': {
      backgroundColor: 'rgba(0, 0, 0, .12)',
    },
  },
  buttonLight: {
    color: colors.primary,
  },
});
