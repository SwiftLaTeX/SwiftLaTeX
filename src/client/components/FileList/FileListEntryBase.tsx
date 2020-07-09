import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';
import FileListEntryDropTarget from './FileListEntryDropTarget';
import { isKeyCombo } from '../shared/KeybindingsManager';
import ContextMenu from '../shared/ContextMenu';
import { Action } from '../shared/ContextMenu';
import { FileSystemEntry } from '../../types';

type Props = {
    entry: FileSystemEntry;
    rest: FileSystemEntry[];
    onOpen: (path: string) => void;
    onFocus: (path: string) => void;
    onRename: (oldPath: string, newPath: string) => void;
    onExpand?: (path: string, expand?: boolean) => void;
    renderMenuIcon: () => React.ReactNode;
    renderItem: () => React.ReactNode;
    renderTree?: () => React.ReactNode;
    actions: (Action | undefined)[];
    draggable?: boolean;
    theme: string | undefined;
};

type State = {
    menu: {
        pageX: number;
        pageY: number;
    } | null;
    isHovered: boolean;
};

export let lastDraggedEntry: FileSystemEntry | null;

export default class FileListEntry extends React.Component<Props, State> {
    static defaultProps = {
        draggable: true,
    };

    state = {
        menu: null,
        isHovered: false,
    };

    componentDidMount() {
        document.addEventListener('click', this._handleDocumentClick);
        document.addEventListener('contextmenu', this._handleDocumentContextMenu);
    }

    componentWillUnmount() {
        document.removeEventListener('click', this._handleDocumentClick);
        document.removeEventListener('contextmenu', this._handleDocumentContextMenu);
    }

    _handleClick = (event: React.MouseEvent<HTMLDivElement>) => {
        if ((event.target as HTMLDivElement).tagName === 'INPUT') {
            return;
        }

        this.props.onOpen(this.props.entry.item.path);
    };

    _handleMouseEnter = () =>
        this.setState({
            isHovered: true,
        });

    _handleMouseLeave = () =>
        this.setState({
            isHovered: false,
        });

    _handleMouseDown = () => (this._click = true);

    _handleFocus = () => {
        if (this._click) {
            // The focus was triggered by a click event
            // Ignore it to avoid double handling
            this._click = false;
            return;
        }

        this.props.onFocus(this.props.entry.item.path);
    };

    _handleKeyDown = (event: React.KeyboardEvent<HTMLDivElement>) => {
        if ((event.target as HTMLDivElement).tagName === 'INPUT') {
            return;
        }

        const bindings = this.props.actions.filter((action) =>
            action && action.combo ? isKeyCombo(event as any, action.combo) : false
        );

        if (bindings.length) {
            event.preventDefault();

            bindings.forEach((binding) => binding && binding.handler());
        }
    };

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
                !this._menu.current.contains(e.target as HTMLElement)
            ) {
                this._hideContextMenu();
            }
        } else if (
            this._more.current &&
            (e.target === this._more.current || this._more.current.contains(e.target as Node))
        ) {
            if (this.state.menu) {
                this._hideContextMenu();
            } else {
                this._showContextMenu(e);
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

    _handleDragStart = (e: React.DragEvent<HTMLDivElement>) => {
        e.dataTransfer.effectAllowed = 'move';
        e.dataTransfer.setData('application/json', JSON.stringify(this.props.entry));
        lastDraggedEntry = this.props.entry;
    };

    _handleDragEnd = () => {
        lastDraggedEntry = null;
    };

    _click: boolean = false;
    _item = React.createRef<HTMLDivElement>();
    _more = React.createRef<HTMLButtonElement>();
    _menu = React.createRef<HTMLUListElement>();

    render() {
        const { entry, rest, draggable, onRename, onExpand, theme, actions } = this.props;
        const { menu, isHovered } = this.state;

        return (
            <FileListEntryDropTarget
                entry={entry}
                rest={rest}
                onRename={onRename}
                onExpand={onExpand}>
                <div
                    ref={this._item}
                    draggable={draggable}
                    onDragStart={this._handleDragStart}
                    onDragEnd={this._handleDragEnd}
                    tabIndex={0}
                    className={css(styles.entry)}
                    onClick={this._handleClick}
                    onMouseEnter={this._handleMouseEnter}
                    onMouseLeave={this._handleMouseLeave}
                    onMouseDown={this._handleMouseDown}
                    onFocus={this._handleFocus}
                    onKeyDown={this._handleKeyDown}>
                    {this.props.renderItem()}
                </div>
                <div
                    className={css(
                        styles.highlight,
                        theme === 'dark' ? styles.highlightDark : styles.highlightLight,
                        entry.state.isSelected ? styles.highlightActive : null
                    )}
                />
                <ContextMenu
                    ref={this._menu}
                    visible={Boolean(menu)}
                    position={menu}
                    actions={actions}
                    onHide={this._hideContextMenu}
                />
                <button
                    ref={this._more}
                    tabIndex={-1}
                    className={css(
                        styles.more,
                        isHovered || menu ? styles.moreVisible : styles.moreInvisible
                    )}>
                    {this.props.renderMenuIcon()}
                </button>
                {this.props.renderTree && this.props.renderTree()}
            </FileListEntryDropTarget>
        );
    }
}

const styles = StyleSheet.create({
    entry: {
        position: 'relative',
        display: 'inline-block',
        outline: 0,
        padding: '4px 16px',
        width: '100%',
        cursor: 'pointer',
        zIndex: 1,
        whiteSpace: 'nowrap',
    },

    highlight: {
        content: '""',
        display: 'inline-block',
        height: 30,
        position: 'absolute',
        left: 0,
        right: 0,
        opacity: 0,
        zIndex: 1,
        pointerEvents: 'none',
    },

    highlightLight: {
        backgroundColor: 'rgba(0, 0, 0, .04)',
    },

    highlightDark: {
        backgroundColor: 'rgba(255, 255, 255, .04)',
    },

    highlightActive: {
        opacity: 1,
    },

    more: {
        position: 'absolute',
        left: -8,
        border: 0,
        outline: 0,
        background: 'none',
        height: 30,
        zIndex: 2,
        padding: '7px 12px',
        textAlign: 'right',

        ':hover': {
            opacity: 1,
        },
    },

    moreInvisible: {
        opacity: 0,
    },

    moreVisible: {
        opacity: 1,
    },

    icon: {
        fill: 'currentColor',
        height: 16,
        width: 16,
    },
});
