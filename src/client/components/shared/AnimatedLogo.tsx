import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';
import withThemeName, { ThemeName } from '../Preferences/withThemeName';
import colors from '../../configs/colors';

type ShapeProps = {
  className: string;
  white: string;
  black: string;
};

const Block = ({ className, white, black }: ShapeProps) => (
  <svg width="82px" height="95px" viewBox="0 0 82 95" className={className}>
    <g
      transform="translate(2.000000, 2.000000)"
      stroke={black}
      strokeLinecap="round"
      strokeLinejoin="round"
      strokeWidth="3">
      <polygon fill={white} points="0 22 40 44 40 91 0 69" />
      <polygon fill={white} points="0 22 39 0 78 22 39 44" />
      <polygon fill={black} points="78 69 40 91 40 44 78 22" />
    </g>
  </svg>
);

const Sphere = ({ className, white, black }: ShapeProps) => (
  <svg width="48px" height="48px" viewBox="0 0 54 54" className={className}>
    <g transform="translate(2.000000, 2.000000)" stroke={black} strokeWidth="3">
      <circle fill={white} cx="24" cy="24" r="24" />
      <path
        d="M44.6358987,12 C41.7645228,28.8507274 27.0454631,41.1867351 9.80745621,41.1896317 C8.85831621,41.1896317 7.9291581,41.1896317 7,41.0707734 C15.5879325,49.5304234 29.1803651,50.3420227 38.7303845,42.9653834 C48.2804039,35.5887441 50.8101654,22.3240171 44.6358987,12 Z"
        fill={black}
      />
    </g>
  </svg>
);

type Props = {
  theme: ThemeName;
};

function AnimatedLogo({ theme }: Props) {
  const white = theme === 'dark' ? colors.content.dark : colors.content.light;
  const black = theme === 'dark' ? colors.text.dark : colors.text.light;

  return (
    <div className={css(styles.container)}>
      <Block white={white} black={black} className={css(styles.item, styles.a)} />
      <Block white={white} black={black} className={css(styles.item, styles.e)} />
      <Block white={white} black={black} className={css(styles.item, styles.b)} />
      <Block white={white} black={black} className={css(styles.item, styles.c)} />
      <Block white={white} black={black} className={css(styles.item, styles.d)} />
      <Block white={white} black={black} className={css(styles.item, styles.f)} />
      <Block white={white} black={black} className={css(styles.item, styles.g)} />
      <Sphere white={white} black={black} className={css(styles.item, styles.o)} />
    </div>
  );
}

export default withThemeName(AnimatedLogo);

const translations = {
  a: 'translate3d(39px, 46.5px, 0)',
  b: 'translate3d(0, 68.5px, 0)',
  c: 'translate3d(78px, 68.5px, 0)',
  d: 'translate3d(39px, 90px, 0)',
  e: 'translate3d(39px, 0px, 0)',
  f: 'translate3d(0, 23px, 0)',
  g: 'translate3d(39px, 42.5px, 0)',
  o: 'translate3d(133px, 12.5px, 0)',
};

const keyframes = {
  a: {
    '0%': { opacity: 0 },
    '39.9%': { opacity: 0 },
    '40%': { transform: translations.c, opacity: 1 },
    '50%': { transform: translations.a },
  },
  b: {},
  d: {
    '0%': { transform: translations.b },
    '20%': { transform: translations.b },
    '30%': { transform: translations.d },
  },
  c: {
    '0%': { opacity: 0 },
    '29.9%': { opacity: 0 },
    '30%': { transform: translations.d, opacity: 1 },
    '40%': { transform: translations.c },
  },
  e: {
    '0%': { opacity: 0 },
    '49.9%': { opacity: 0 },
    '50%': { transform: translations.a, opacity: 1 },
    '60%': { transform: translations.e },
  },
  f: {
    '0%': { opacity: 0 },
    '59.9%': { opacity: 0 },
    '60%': { transform: translations.e, opacity: 1 },
    '70%': { transform: translations.f },
  },
  g: {
    '0%': { opacity: 0 },
    '69.9%': { opacity: 0 },
    '70%': { transform: translations.f, opacity: 1 },
    '80%': { transform: translations.g },
  },
  o: {
    '0%': { transform: `${translations.o} scale(0)` },
    '80%': { transform: `${translations.o} scale(0)` },
    '90%': { transform: `${translations.o} scale(1)` },
  },
};

const styles = StyleSheet.create({
  container: {
    position: 'relative',
    height: 182,
    width: 182,
    overflow: 'hidden',
  },
  item: {
    position: 'absolute',
    top: 0,
    left: 0,
    animationDuration: '4s',
    animationDelay: '1s',
    animationIterationCount: 'infinite',
    animationDirection: 'alternate-reverse',
  },
  a: {
    transform: translations.a,
    animationName: keyframes.a,
  },
  b: {
    transform: translations.b,
    animationName: keyframes.b,
  },
  c: {
    transform: translations.c,
    animationName: keyframes.c,
  },
  d: {
    transform: translations.d,
    animationName: keyframes.d,
  },
  e: {
    transform: translations.e,
    animationName: keyframes.e,
  },
  f: {
    transform: translations.f,
    animationName: keyframes.f,
  },
  g: {
    transform: translations.g,
    animationName: keyframes.g,
  },
  o: {
    transform: translations.o,
    animationName: keyframes.o,
  },
});
