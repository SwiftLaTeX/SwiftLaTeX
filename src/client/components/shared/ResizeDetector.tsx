import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';

type Props = {
  onResize: () => void;
  children: React.ReactNode;
};

export default class ResizeDetector extends React.Component<Props> {
  componentDidMount() {
    const horiz = this._horizontal.current;
    const verti = this._vertical.current;

    horiz &&
      horiz.contentWindow &&
      horiz.contentWindow.addEventListener('resize', this._handleResize);

    verti &&
      verti.contentWindow &&
      verti.contentWindow.addEventListener('resize', this._handleResize);
  }

  componentWillUnmount() {
    const horiz = this._horizontal.current;
    const verti = this._vertical.current;

    horiz &&
      horiz.contentWindow &&
      horiz.contentWindow.removeEventListener('resize', this._handleResize);

    verti &&
      verti.contentWindow &&
      verti.contentWindow.removeEventListener('resize', this._handleResize);
  }

  _handleResize = () => this.props.onResize();

  _horizontal = React.createRef<HTMLIFrameElement>();
  _vertical = React.createRef<HTMLIFrameElement>();

  render() {
    return (
      <div className={css(styles.container)}>
        {/* pointer-events: none is not working properly on EDGE, so we render 2 iframes to detect resize instead of one iframe covering the entire editor */}
        <iframe ref={this._horizontal} className={css(styles.phantom, styles.horizontal)} />
        <iframe ref={this._vertical} className={css(styles.phantom, styles.vertical)} />
        {this.props.children}
      </div>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    display: 'flex',
    flex: 1,
    flexDirection: 'column',
    minWidth: 0,
    minHeight: 0,
    position: 'relative',
    height: '100%'
  },
  phantom: {
    display: 'block',
    position: 'absolute',
    left: 0,
    top: 0,
    pointerEvents: 'none',
    opacity: 0,
  },
  horizontal: {
    height: 1,
    width: '100%',
  },
  vertical: {
    height: '100%',
    width: 1,
  },
});
