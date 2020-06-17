import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';

type Props = {
  inverted?: boolean;
  direction: 'horizontal' | 'vertical';
  children?: React.ReactNode;
  className?: string;
};

type State = {
  resizing: boolean;
  initialPosition: {
    pageX: number;
    pageY: number;
  } | null;
  initialWidth: number;
  initialHeight: number;
};

export default class ResizablePane extends React.PureComponent<Props, State> {
  state: State = {
    resizing: false,
    initialPosition: null,
    initialWidth: 0,
    initialHeight: 0,
  };

  componentDidMount() {
    // @ts-ignore
    document.addEventListener('mouseup', this._handleMouseUp);
    // @ts-ignore
    document.addEventListener('mousemove', this._handleMouseMove);

    // if (this._pane.current) {
    //   const { direction } = this.props;
    //   const stylePackage = getComputedStyle(this._pane.current);
    //   let style;
    //   if (direction === 'horizontal') {
    //     style = `width: ${stylePackage.width} !important;`;
    //   } else {
    //     style = `height: ${stylePackage.height} !important`;
    //   }
    //   // console.log(style);
    //   this._pane.current.setAttribute('style', style);
    //   return;
    // }
  }

  componentWillUnmount() {
    // @ts-ignore
    document.removeEventListener('mouseup', this._handleMouseUp);
    // @ts-ignore
    document.removeEventListener('mousemove', this._handleMouseMove);
  }

  _handleMouseDown = (e: React.MouseEvent<HTMLDivElement>) => {
    if (!this._pane.current) {
      return;
    }

    const style = getComputedStyle(this._pane.current);
    this.setState({
      resizing: true,
      initialPosition: {
        pageX: e.pageX,
        pageY: e.pageY,
      },
      initialWidth: parseFloat(style.width || '0'),
      initialHeight: parseFloat(style.height || '0'),
    });
  };

  _handleMouseUp = (e: React.MouseEvent<HTMLDivElement>) => {
    if (this.state.resizing) {
      e.preventDefault();
      this.setState({
        resizing: false,
        initialPosition: null,
        initialWidth: 0,
        initialHeight: 0,
      });
    }
  };

  _handleMouseMove = (e: React.MouseEvent<HTMLDivElement>) => {
    const { direction, inverted } = this.props;
    const { resizing, initialPosition, initialWidth, initialHeight } = this.state;

    if (resizing && initialPosition) {
      e.preventDefault();

      let style;

      if (direction === 'horizontal') {
        const delta = e.pageX - initialPosition.pageX;

        style = `width: ${initialWidth + (inverted ? -delta : delta)}px !important;`;
      } else {
        const delta = initialPosition.pageY - e.pageY;

        style = `height: ${initialHeight + (inverted ? -delta : delta)}px !important`;
      }

      this._pane.current && this._pane.current.setAttribute('style', style);
    }
  };

  _pane = React.createRef<HTMLDivElement>();

  render() {
    const { direction, inverted, className, children } = this.props;

    return (
      <div ref={this._pane} className={`${css(styles.container)} ${className || ''}`}>
        {children}
        <div
          className={css(
            styles.handle,
            direction === 'horizontal' ? styles.horizontal : styles.vertical,
            direction === 'horizontal'
              ? inverted
                ? styles.horizontalInverted
                : styles.horizontalNormal
              : inverted
              ? styles.verticalInverted
              : styles.verticalNormal
          )}
          onMouseDown={this._handleMouseDown}
          onMouseUp={this._handleMouseUp}
        />
      </div>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    position: 'relative',
  },
  handle: {
    position: 'absolute',
    zIndex: 1,
  },
  horizontal: {
    top: 0,
    bottom: 0,
    width: 12,
    cursor: 'col-resize',
  },
  vertical: {
    left: 0,
    right: 0,
    height: 12,
    cursor: 'row-resize',
  },
  horizontalNormal: {
    right: -12,
  },
  horizontalInverted: {
    left: -12,
  },
  verticalNormal: {
    top: -12,
  },
  verticalInverted: {
    bottom: -12,
  },
});
