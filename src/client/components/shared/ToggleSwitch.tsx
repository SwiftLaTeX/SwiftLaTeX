import * as React from 'react';
import classnames from 'classnames';
import { StyleSheet, css } from 'aphrodite';
import colors from '../../configs/colors';
import usePreferences from '../Preferences/usePreferences';

type Props = {
  checked: boolean;
  label: string;
  onChange: (event: React.ChangeEvent<HTMLInputElement>) => void;
  className?: string;
};

export default function ToggleSwitch(props: Props) {
  const [prefs] = usePreferences();

  return (
    <label className={classnames(css(styles.container), props.className)}>
      <span className={css(styles.label)}>{props.label}</span>
      <span
        className={css(
          styles.switch,
          props.checked ? styles.active : styles.inactive,
          prefs.theme === 'dark' ? styles.dark : styles.light,
          prefs.theme === 'dark'
            ? props.checked
              ? styles.darkActive
              : styles.darkInactive
            : props.checked
            ? styles.lightActive
            : styles.lightInactive
        )}
      />
      <input
        type="checkbox"
        checked={props.checked}
        onChange={props.onChange}
        className={css(styles.check)}
      />
    </label>
  );
}

const styles = StyleSheet.create({
  container: {
    display: 'flex',
    alignItems: 'center',
    margin: 8,
    cursor: 'pointer',
    whiteSpace: 'nowrap',
  },
  switch: {
    display: 'inline-block',
    verticalAlign: -4,
    width: 36,
    height: 20,
    borderRadius: 12,

    ':before': {
      content: '""',
      display: 'inline-block',
      height: 14,
      width: 14,
      borderRadius: 7,
      margin: 2,
      transition: '.2s',
      transform: 'translateX(0)',
    },
  },
  inactive: {
    ':before': {
      transform: 'translateX(0)',
    },
  },
  active: {
    ':before': {
      transform: 'translateX(16px)',
    },
  },
  light: {
    border: `1px solid ${colors.border}`,
  },
  lightInactive: {
    ':before': {
      backgroundColor: 'rgba(0, 0, 0, .5)',
    },
  },
  lightActive: {
    ':before': {
      backgroundColor: colors.primary,
    },
  },
  dark: {
    border: '1px solid rgba(255, 255, 255, .2)',
  },
  darkInactive: {
    ':before': {
      backgroundColor: 'rgba(255, 255, 255, .5)',
    },
  },
  darkActive: {
    ':before': {
      backgroundColor: colors.content.light,
    },
  },
  check: {
    display: 'none',
  },
  label: {
    flex: 1,
    padding: '0 .5em',
    fontWeight: 'normal',
  },
});
