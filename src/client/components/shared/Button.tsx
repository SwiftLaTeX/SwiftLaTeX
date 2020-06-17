import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';
import classnames from 'classnames';
import withThemeName, { ThemeName } from '../Preferences/withThemeName';

import colors from '../../configs/colors';

export type ButtonCommonProps = {
  variant?: 'primary' | 'secondary' | 'accent';
  large?: boolean;
  icon?: string;
  disabled?: boolean;
  loading?: boolean;
  theme: ThemeName;
};

type Props = ButtonCommonProps & {
  type?: 'submit' | 'button';
  className?: string;
  onClick?: (event: React.MouseEvent<HTMLButtonElement>) => void;
  children: React.ReactNode;
};

const rotate = {
  from: { transform: 'rotate(0deg)' },
  to: { transform: 'rotate(360deg)' },
};

export const getClassNames = ({
  variant,
  icon,
  large,
  disabled,
  loading,
  theme,
}: ButtonCommonProps) => {
  return css(
    styles.button,
    variant === 'primary' || variant === 'secondary'
      ? styles[variant]
      : variant === 'accent'
      ? theme === 'dark'
        ? styles.accentDark
        : styles.accentLight
      : theme === 'dark'
      ? styles.normalDark
      : styles.normalLight,
    icon ? styles.iconButton : large ? styles.largeButton : styles.normalButton,
    loading && styles.loading,
    disabled && styles.disabled,
    disabled && (variant ? styles.primaryDisabled : styles.normalDisabled)
  );
};

function Button({ variant, icon, large, disabled, loading, className, theme, ...rest }: Props) {
  return (
    <button
      type="button"
      className={classnames(
        getClassNames({ variant, icon, large, disabled, loading, theme }),
        className
      )}
      disabled={disabled}
      style={icon ? { backgroundImage: `url(${icon})` } : {}}
      {...rest}
    />
  );
}

export default withThemeName(Button);

const styles = StyleSheet.create({
  button: {
    appearance: 'none',
    backgroundColor: 'transparent',
    color: 'inherit',
    outline: 0,
    borderRadius: 3,
    whiteSpace: 'nowrap',
    textAlign: 'center',
    textDecoration: 'none',
    transitionDuration: '170ms',
    transitionProperty: 'box-shadow',
    transitionTimingFunction: 'linear',

    ':hover': {
      boxShadow: '0 4px 16px rgba(0, 1, 31, 0.2)',
    },
  },

  primary: {
    color: '#fff',
    backgroundColor: colors.primary,
    border: `1px solid transparent`,
  },

  secondary: {
    color: '#fff',
    backgroundColor: colors.secondary,
    border: `1px solid transparent`,
  },

  accentLight: {
    color: '#fff',
    backgroundColor: colors.accent.light,
    border: `1px solid transparent`,
  },

  accentDark: {
    color: '#000',
    backgroundColor: colors.accent.dark,
    border: `1px solid transparent`,
  },

  normalLight: {
    color: colors.text.light,
    backgroundColor: '#fff',
    border: `1px solid ${colors.border}`,
  },

  normalDark: {
    color: colors.text.dark,
    backgroundColor: 'rgba(255, 255, 255, .08)',
    borderColor: `transparent`,
  },

  disabled: {
    cursor: 'not-allowed',
    pointerEvents: 'none',
  },

  normalDisabled: {
    backgroundColor: 'rgba(0, 0, 0, .02)',
    color: '#999',
    border: `1px solid ${colors.border}`,
  },

  primaryDisabled: {
    opacity: 0.3,
    borderColor: `transparent`,
  },

  iconButton: {
    padding: '.5em 1em .5em 36px',
    margin: '.5em',
    backgroundSize: 16,
    backgroundRepeat: 'no-repeat',
    backgroundPosition: '12px center',
  },

  normalButton: {
    padding: '.5em 1em',
    margin: '.5em',
  },

  largeButton: {
    fontSize: 16,
    padding: '1em 1.5em',
    margin: '.5em 0',
    width: '100%',
  },

  loading: {
    ':before': {
      display: 'inline-block',
      content: '""',
      borderWidth: 2,
      borderStyle: 'solid',
      borderTopColor: 'rgba(255, 255, 255, 0.2)',
      borderRightColor: 'rgba(255, 255, 255, 0.2)',
      borderBottomColor: 'rgba(255, 255, 255, 0.2)',
      borderLeftColor: '#fff',
      height: 16,
      width: 16,
      borderRadius: '50%',
      marginRight: '.75em',
      verticalAlign: -3,
      animationName: [rotate],
      animationDuration: '1s',
      animationIterationCount: 'infinite',
      animationTimingFunction: 'linear',
    },
  },
});
