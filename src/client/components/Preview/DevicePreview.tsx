import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';
import classnames from 'classnames';
import withThemeName, { ThemeName } from '../Preferences/withThemeName';
// import ToggleButtons from '../shared/ToggleButtons';
import WebFrame from './WebFrame';

// import * as PlatformOptions from '../../utils/PlatformOptions';

export const VISIBILITY_MEDIA_QUERY = '(min-width: 700px)';

type Props = {
  className?: string;
  previewRef: React.MutableRefObject<Window | null>;
  screenOnly?: boolean;
  theme: ThemeName;
  onPreviewClick: (path: string, line:number, column:number) => void;
};

type State = {
  popupUrl?: string;
  isRendered: boolean;
  isPopupOpen: boolean;
};

class DevicePreview extends React.PureComponent<Props, State> {
  state: State = {
    isRendered: false,
    isPopupOpen: false,
  };

  componentDidMount() {
    this.mql = window.matchMedia(VISIBILITY_MEDIA_QUERY);
    this.mql.addListener(this.handleMediaQuery);
    this.handleMediaQuery(this.mql);
  }

  componentDidUpdate(_: Props, prevState: State) {
    if (this.state.isPopupOpen && prevState.popupUrl !== this.state.popupUrl) {
      this.handlePopup();
    }
  }

  componentWillUnmount() {
    clearInterval(this.popupInterval);

    this.mql && this.mql.removeListener(this.handleMediaQuery);

    if (this.popup) {
      this.popup.close();
    }
  }

  private handleMediaQuery = (mql: any) =>
    this.setState({
      isRendered: mql.matches,
    });

  private handlePopup = () => {
    this.popup = window.open(this.state.popupUrl, 'snack-device', 'width=327,height=668');

    if (this.popup && this.popup.closed) {
      return;
    }

    this.setState(
      {
        isPopupOpen: true,
      },
      () => {
        this.props.previewRef.current = this.popup;
      }
    );

    const handler = (ev: any) => {
      if(ev.data.cmd === 'setCursor') {
        this.props.onPreviewClick(ev.data.path, ev.data.line, ev.data.column);
      }
    }

    window.addEventListener("message", handler);
    window.name = 'parent-window';

    clearInterval(this.popupInterval);

    this.popupInterval = setInterval(() => {
      if (!this.popup || this.popup.closed) {
        clearInterval(this.popupInterval);
        window.removeEventListener("message", handler);
        this.popup = null;
        this.setState({
          isPopupOpen: false,
        });
      }
    }, 500);
  };

  private handlePopupUrl = (url: string) => this.setState({ popupUrl: url });

  private popupInterval: any;
  private popup: Window | null = null;
  private mql: MediaQueryList | null = null;

  render() {
    const { isPopupOpen, isRendered } = this.state;

    if (!isRendered || isPopupOpen) {
      return null;
    }

    const { previewRef, screenOnly, theme } = this.props;

    return (
      <div
        className={classnames(
          css(screenOnly ? styles.centered : styles.container),
          this.props.className
        )}>
        {screenOnly ? null : (
          <div className={css(styles.header)}>
            <button
              className={css(
                styles.popupButton,
                theme === 'dark' ? styles.popupButtonDark : styles.popupButtonLight
              )}
              onClick={this.handlePopup}
            />
          </div>
        )}
        <WebFrame previewRef={previewRef} onPopupUrl={this.handlePopupUrl} onPreviewClick={this.props.onPreviewClick} />
      </div>
    );
  }
}

export default withThemeName(DevicePreview);

const styles = StyleSheet.create({
  container: {
    position: 'relative',
    overflow: 'auto',
    display: 'flex',
    flexDirection: 'column',
    flex: 1,
  },
  header: {
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
    margin: '8px 0',
  },
  toggleButtons: {
    zIndex: 3,
  },
  popupButton: {
    position: 'absolute',
    right: 0,
    zIndex: 2,
    appearance: 'none',
    height: 48,
    width: 48,
    padding: 16,
    margin: 0,
    border: 0,
    outline: 0,
    opacity: 0.8,
    backgroundSize: 16,
    backgroundPosition: 'center',
    backgroundRepeat: 'no-repeat',
    backgroundColor: 'transparent',
    transition: '.2s',

    ':hover': {
      opacity: 1,
    },
  },
  popupButtonDark: {
    backgroundImage: `url(${require('../../assets/open-link-icon-light.png')})`,
  },
  popupButtonLight: {
    backgroundImage: `url(${require('../../assets/open-link-icon.png')})`,
  },
  centered: {
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'center',
    overflow: 'auto',
  },
});
