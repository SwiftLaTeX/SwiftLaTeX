import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';
import FileListEntryIcon from './FileListEntryIcon';
import ContextMenu from '../shared/ContextMenu';
import withThemeName, { ThemeName } from '../Preferences/withThemeName';
import { FileSystemEntry } from '../../types';

type Props = {
    entry: FileSystemEntry;
    entryPoint: string;
    onOpen: () => void;
    onClose: () => void;
    onCloseOthers: () => void;
    onCloseAll: () => void;
    theme: ThemeName;
};

type State = {
    isHovered: boolean;
    menu:
        | {
              pageX: number;
              pageY: number;
          }
        | undefined
        | null;
};

class FileListOpenEntry extends React.PureComponent<Props, State> {
    state: State = {
        isHovered: false,
        menu: null,
    };

    componentDidMount() {
        document.addEventListener('click', this._handleDocumentClick);
        document.addEventListener('contextmenu', this._handleDocumentContextMenu);
    }

    componentWillUnmount() {
        document.removeEventListener('click', this._handleDocumentClick);
        document.removeEventListener('contextmenu', this._handleDocumentContextMenu);
    }

    _hideContextMenu = () => this.setState({ menu: null });

    _showContextMenu = (e: MouseEvent) => {
        this.setState({
            menu: {
                pageX: e.pageX,
                pageY: e.pageY,
            },
        });
    };

    _handleDocumentClick = (e: MouseEvent) => {
        if (this.state.menu) {
            if (
                this._menu.current &&
                e.target !== this._menu.current &&
                !this._menu.current.contains(e.target as Node)
            ) {
                this._hideContextMenu();
            }
        }
    };

    _handleDocumentContextMenu = (e: MouseEvent) => {
        if (
            e.target === this._item.current ||
            (this._item.current && this._item.current.contains(e.target as Node))
        ) {
            e.preventDefault();
            this._showContextMenu(e);
        } else if (this.state.menu) {
            this._hideContextMenu();
        }
    };

    _handleMouseEnter = () =>
        this.setState({
            isHovered: true,
        });

    _handleMouseLeave = () =>
        this.setState({
            isHovered: false,
        });

    _menu = React.createRef<HTMLUListElement>();
    _item = React.createRef<HTMLLIElement>();

    render() {
        const { entry, theme, entryPoint } = this.props;
        const displayName = entry.item.path.split('/').pop();

        return (
            <li
                ref={this._item}
                tabIndex={-1}
                className={css(
                    styles.item,
                    entry.state.isFocused &&
                        (theme === 'dark' ? styles.focusedDark : styles.focusedLight)
                )}
                onMouseEnter={this._handleMouseEnter}
                onMouseLeave={this._handleMouseLeave}>
                <button
                    onClick={this.props.onClose}
                    className={css(
                        styles.close,
                        this.state.isHovered ? styles.closeFocused : styles.closeBlurred
                    )}>
                    ×
                </button>
                <div onClick={this.props.onOpen}>
                    <FileListEntryIcon entry={entry} entryPoint={entryPoint} />
                    <span className={css(styles.label)}>{displayName}</span>
                </div>
                <ContextMenu
                    ref={this._menu}
                    visible={Boolean(this.state.menu)}
                    position={this.state.menu}
                    actions={[
                        {
                            label: 'Close',
                            handler: this.props.onClose,
                        },
                        {
                            label: 'Close Others',
                            handler: this.props.onCloseOthers,
                        },
                        {
                            label: 'Close All',
                            handler: this.props.onCloseAll,
                        },
                    ]}
                    onHide={this._hideContextMenu}
                />
            </li>
        );
    }
}

export default withThemeName(FileListOpenEntry);

const styles = StyleSheet.create({
    item: {
        paddingLeft: 24,
        cursor: 'pointer',
        outline: 0,
        whiteSpace: 'nowrap',
    },
    label: {
        display: 'inline-block',
        verticalAlign: -1,
        margin: 6,
        userSelect: 'none',
        whiteSpace: 'nowrap',
    },
    focusedLight: {
        backgroundColor: 'rgba(0, 0, 0, .04)',
    },
    focusedDark: {
        backgroundColor: 'rgba(255, 255, 255, .04)',
    },
    close: {
        padding: '7px 8px',
        fontSize: '18px',
        border: 'none',
        appearance: 'none',
        position: 'absolute',
        left: 0,
        margin: '-3px 0 0 0',
        background: 'none',
        outline: 0,
    },
    closeFocused: {
        opacity: 1,
    },
    closeBlurred: {
        opacity: 0,
    },
});
