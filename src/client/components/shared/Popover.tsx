import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';

import withThemeName, { ThemeName } from '../Preferences/withThemeName';
import colors from '../../configs/colors';

type Props = {
    children: React.ReactNode;
    content: React.ReactNode;
    theme: ThemeName;
};

type State = {
    visible: boolean;
};

class Popover extends React.PureComponent<Props, State> {
    state = {
        visible: false,
    };

    componentDidMount() {
        document.addEventListener('click', this._handleDocumentClick);
        document.addEventListener('contextmenu', this._handleDocumentContextMenu);
    }

    componentWillUnmount() {
        document.removeEventListener('click', this._handleDocumentClick);
        document.removeEventListener('contextmenu', this._handleDocumentContextMenu);
    }

    _handleDocumentContextMenu = () => {
        if (this.state.visible) {
            this._hidePopover();
        }
    };

    _handleDocumentClick = (e: MouseEvent) => {
        if (
            this.state.visible &&
            (e.target === this._anchor.current ||
                e.target === this._popover.current ||
                (this._popover.current && this._popover.current.contains(e.target as Node)))
        ) {
            return;
        }

        this._hidePopover();
    };

    _togglePopover = () => {
        if (!this.state.visible) {
            const popover =
                (this._popover.current && this._popover.current.getBoundingClientRect()) || {};
            const anchor =
                (this._anchor.current && this._anchor.current.getBoundingClientRect()) || {};

            // @ts-ignore
            const diff = (popover.width - 10) / 2 - anchor.left;

            if (this._popover.current && this._arrow.current) {
                if (diff > 0) {
                    this._popover.current.style.left = `${diff + 5}px`;
                    this._arrow.current.style.left =
                        // @ts-ignore
                        `${anchor.left - anchor.width / 2 + 10}px`;
                } else {
                    this._popover.current.style.left = '5px';
                    this._arrow.current.style.left = '50%';
                }
            }
        }

        this.setState((state) => ({ visible: !state.visible }));
    };

    _hidePopover = () => this.setState({ visible: false });

    _anchor = React.createRef<HTMLElement>();
    _arrow = React.createRef<HTMLSpanElement>();
    _popover = React.createRef<HTMLDivElement>();

    render() {
        const { children, content, theme } = this.props;

        return (
            <div className={css(styles.container)}>
                {React.cloneElement(
                    // @ts-ignore
                    React.Children.only(children),
                    {
                        ref: this._anchor,
                        onClick: this._togglePopover,
                    }
                )}
                <div
                    ref={this._popover}
                    className={css(
                        styles.popover,
                        theme === 'dark' ? styles.popoverDark : styles.popoverLight,
                        this.state.visible ? styles.visible : styles.hidden
                    )}>
                    <span
                        ref={this._arrow}
                        className={css(
                            styles.arrow,
                            theme === 'dark' ? styles.arrowDark : styles.arrowLight
                        )}
                    />
                    {content}
                </div>
            </div>
        );
    }
}

export default withThemeName(Popover);

const styles = StyleSheet.create({
    container: {
        position: 'relative',
        backgroundColor: 'inherit',
    },

    popover: {
        position: 'absolute',
        top: '100%',
        margin: 12,
        width: '18em',
        borderRadius: 3,
        zIndex: 99,
        backgroundColor: 'inherit',
        color: 'inherit',
        transition: 'transform .2s, opacity .2s',
        boxShadow: '0 2px 6px rgba(0, 0, 0, 0.16), 0 0 3px rgba(0, 0, 0, 0.08)',
    },

    popoverLight: {
        backgroundColor: colors.content.light,
        border: 0,
    },

    popoverDark: {
        backgroundColor: colors.content.dark,
        border: `1px solid ${colors.ayu.mirage.border}`,
    },

    arrow: {
        position: 'absolute',
        height: 16,
        width: 16,
        top: -9,
        transform: 'translateX(-50%) rotate(45deg)',
        backgroundColor: 'inherit',
        borderTopLeftRadius: 4,
        boxShadow: '-.5px -.5px 0 rgba(0, 0, 0, .12)',
    },

    arrowLight: {
        border: 0,
    },

    arrowDark: {
        borderStyle: 'solid',
        borderWidth: '1px 0 0 1px',
        borderColor: colors.ayu.mirage.border,
    },

    visible: {
        opacity: 1,
        transform: 'translateX(-50%) translateY(0)',
    },

    hidden: {
        opacity: 0,
        pointerEvents: 'none',
        transform: 'translateX(-50%) translateY(-4px)',
    },
});
