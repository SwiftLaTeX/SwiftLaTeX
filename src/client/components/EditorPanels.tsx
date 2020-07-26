import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';
import ResizablePane from './shared/ResizablePane';
import EditorPanelLogs from './EditorPanelLogs';
import EditorPanelError from './EditorPanelError';
import colors from '../configs/colors';
import { Annotation } from '../types';


type Props = {
    annotations: Annotation[];
    deviceLogs: string;
    onShowErrorPanel: () => void;
    onShowDeviceLogs: () => void;
    onTogglePanels: () => void;
    panelType: 'errors' | 'logs';
};

export default class EditorPanels extends React.Component<Props> {
    getSnapshotBeforeUpdate(prevProps: Props) {
        if (this.props.deviceLogs !== prevProps.deviceLogs && this._panel.current) {
            this._isScrolled =
                this._panel.current.scrollHeight - this._panel.current.clientHeight !==
                this._panel.current.scrollTop;
        }
        return null;
    }

    componentDidUpdate(prevProps: Props) {
        if (
            this.props.deviceLogs !== prevProps.deviceLogs &&
            this._panel.current &&
            !this._isScrolled
        ) {
            this._panel.current.scrollTop =
                this._panel.current.scrollHeight - this._panel.current.clientHeight;
        }
    }

    _isScrolled: boolean = false;
    _panel = React.createRef<HTMLDivElement>();

    render() {
        const {
            annotations,
            deviceLogs,
            onShowErrorPanel,
            onShowDeviceLogs,
            onTogglePanels,
            panelType,
        } = this.props;
        return (
            <ResizablePane direction="vertical" className={css(styles.container)}>
                <div className={css(styles.panels)}>
                    <div className={css(styles.header)}>
                        <button
                            onClick={onShowErrorPanel}
                            className={css(styles.tab, panelType !== 'errors' && styles.inactive)}>
                            Errors
                        </button>
                        <button
                            onClick={onShowDeviceLogs}
                            className={css(styles.tab, panelType !== 'logs' && styles.inactive)}>
                            Logs
                        </button>
                        <div className={css(styles.buttons)}>
                            <button
                                onClick={onTogglePanels}
                                className={css(styles.button, styles.close)}
                            />
                        </div>
                    </div>
                    <div ref={this._panel} className={css(styles.panel)}>
                        {panelType === 'errors' ? (
                            <EditorPanelError annotations={annotations} />
                        ) : null}
                        {panelType === 'logs' ? <EditorPanelLogs deviceLogs={deviceLogs} /> : null}
                    </div>
                </div>
            </ResizablePane>
        );
    }
}

const styles = StyleSheet.create({
    container: {
        height: '14em',
    },

    header: {
        display: 'flex',
        flexDirection: 'row',
        alignItems: 'center',
        paddingTop: '.75em',
    },

    tab: {
        display: 'inline-block',
        appearance: 'none',
        background: 'none',
        border: 'none',
        margin: 0,
        padding: '.35em 1.5em',
        fontSize: '.9em',
        textTransform: 'uppercase',
        outline: 'none',
        opacity: 1,
    },

    inactive: {
        opacity: 0.5,
    },

    buttons: {
        display: 'flex',
        flex: 1,
        flexDirection: 'row',
        justifyContent: 'flex-end',
        margin: '0 1em',
    },

    button: {
        height: 24,
        width: 24,
        border: 0,
        outline: 0,
        margin: '0 .5em',
        appearance: 'none',
        backgroundColor: 'transparent',
        backgroundSize: 16,
        backgroundRepeat: 'no-repeat',
        backgroundPosition: 'center',
    },

    close: {
        backgroundImage: `url(${require('../assets/cross.png')})`,
    },

    clear: {
        backgroundImage: `url(${require('../assets/clear.png')})`,
    },

    panels: {
        borderColor: colors.border,
        borderWidth: '1px 0 0 0',
        borderStyle: 'solid',
        height: '100%',
        minHeight: 0,
    },

    panel: {
        display: 'flex',
        flexDirection: 'column',
        minHeight: 0,
        padding: '.5em 1.5em .75em 1.5em',
        overflow: 'auto',
        height: 'calc(100% - 2.5em)',
    },

    error: {
        color: colors.error,
    },

    warning: {
        color: colors.warning,
    },

    line: {
        border: 0,
        margin: 0,
        padding: '4px 0',
        backgroundColor: 'transparent',
        fontFamily: 'var(--font-monospace)',
        color: 'inherit',
    },
});
