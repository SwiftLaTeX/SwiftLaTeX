import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';
import ToggleSwitch from './shared/ToggleSwitch';
import FooterShell from './Shell/FooterShell';
import MenuButton from './shared/MenuButton';
import IconButton from './shared/IconButton';
import colors from '../configs/colors';
import { Annotation } from '../types';
import { c } from './ColorsProvider';
import EngineVersionSwitcher from './EngineVersionSwitcher';
import { EngineVersion } from '../types';

type Props = {
    annotations: Annotation[];
    fileTreeShown: boolean;
    devicePreviewShown: boolean;
    panelsShown: boolean;
    sendCodeOnChangeEnabled: boolean;
    onToggleTheme: () => void;
    onTogglePanels: () => void;
    onToggleFileTree: () => void;
    onToggleDevicePreview: () => void;
    onToggleSendCode: () => void;
    // onShowShortcuts: () => void;
    onSendCode: () => void;
    // onReloadSnack: () => void;
    theme: string;
    engine: EngineVersion,
    onChangeEngineVersion: (engine: EngineVersion) => Promise<void>;
};

export default function Footer(props: Props) {
    const {
        annotations,
        fileTreeShown,
        devicePreviewShown,
        panelsShown,
        sendCodeOnChangeEnabled,
        // editorMode,
        // sdkVersion,
        onSendCode,
        onToggleTheme,
        onTogglePanels,
        onToggleFileTree,
        onToggleDevicePreview,
        onToggleSendCode,
        engine,
        onChangeEngineVersion,
        // onShowShortcuts,
        theme,
    } = props;

    const isErrorFatal = annotations.some(a => a.severity > 3);

    return (
        <FooterShell type={isErrorFatal ? 'error' : null}>
            <div className={css(styles.left)}>
                {
                    <button
                        onClick={onTogglePanels}
                        className={css(
                            styles.statusText,
                            annotations.length
                                ? isErrorFatal
                                ? styles.errorTextFatal
                                : styles.errorText
                                : styles.successText,
                        )}>
                        {annotations.length
                            ? `${annotations[0].source}: ${annotations[0].message.split('\n')[0]}` +
                            (annotations.length > 1 ? ` (+${annotations.length - 1} more)` : '')
                            : 'No errors'}
                    </button>
                }
            </div>
            {/*<FooterButton icon={require('../assets/prettify-icon.png')} onClick={onPrettifyCode}>
        <span className={css(styles.buttonLabel)}>Prettier</span>
      </FooterButton>*/}
            <EngineVersionSwitcher sdkVersion={engine} onChange={onChangeEngineVersion}/>
            <MenuButton
                icon={require('../assets/settings-icon.png')}
                label={
                    <React.Fragment>
                        <span className={css(styles.buttonLabel)}>Compiler Settings</span>
                    </React.Fragment>
                }
                content={
                    <div className={css(styles.devicePane)}>
                        {
                            <React.Fragment>
                                <div className={css(styles.devicePaneItem)}>
                                    <ToggleSwitch
                                        checked={sendCodeOnChangeEnabled}
                                        onChange={onToggleSendCode}
                                        label="Update as you type"
                                    />
                                </div>
                                <div className={css(styles.buttonItem, styles.buttonItemDevicePane)}>
                                    <IconButton title={`Update changes`} label="Force Recompilation"
                                                onClick={onSendCode}>
                                        <svg width="14px" height="17px" viewBox="0 0 14 17">
                                            <path
                                                transform="translate(-5.000000, -3.000000)"
                                                d="M9,16 L15,16 L15,10 L19,10 L12,3 L5,10 L9,10 L9,16 Z M5,18 L19,18 L19,20 L5,20 L5,18 Z"
                                            />
                                        </svg>
                                    </IconButton>
                                </div>
                            </React.Fragment>
                        }
                    </div>
                }
            />
            <MenuButton
                icon={require('../assets/edit-icon.png')}
                label={<span className={css(styles.buttonLabel)}>Editor Settings</span>}
                content={
                    <React.Fragment>
                        <div className={css(styles.buttonItem, styles.buttonItemEditorPane)}>
                        </div>
                        <ToggleSwitch checked={fileTreeShown} onChange={onToggleFileTree} label="Files"/>
                        <ToggleSwitch checked={panelsShown} onChange={onTogglePanels} label="Panel"/>
                        <ToggleSwitch checked={theme !== 'light'} onChange={onToggleTheme} label="Dark theme"/>
                        <ToggleSwitch
                            checked={devicePreviewShown}
                            onChange={onToggleDevicePreview}
                            label="Preview"
                        />
                    </React.Fragment>
                }
            />


        </FooterShell>
    );
}

const styles = StyleSheet.create({
    left: {
        display: 'flex',
        alignItems: 'stretch',
        flex: 1,
        minWidth: 0,
        overflow: 'hidden',
    },

    loadingText: {
        textOverflow: 'ellipsis',
        whiteSpace: 'nowrap',
        padding: '.5em',
    },

    statusText: {
        border: 0,
        outline: 0,
        margin: 0,
        appearance: 'none',
        backgroundColor: 'transparent',
        backgroundSize: 16,
        backgroundRepeat: 'no-repeat',
        backgroundPosition: '1em center',
        display: 'inline-block',
        textOverflow: 'ellipsis',
        whiteSpace: 'nowrap',
        padding: '.25em .5em .25em 3em',
        minWidth: 200,
        width: '100%',
        textAlign: 'left',
    },

    successText: {
        backgroundImage: `url(${require('../assets/checkmark.png')})`,
    },

    errorText: {
        backgroundImage: `url(${require('../assets/cross-red.png')})`,
        color: colors.error,
    },

    errorTextFatal: {
        backgroundImage: `url(${require('../assets/cross-light.png')})`,
    },

    devicesCount: {
        position: 'absolute',
        top: 4,
        right: 6,
        height: 20,
        minWidth: 20,
        borderRadius: '50%',
        backgroundColor: c('text'),
        color: c('background'),
        opacity: 0.5,
    },

    devicePane: {
        padding: '0 16px',
    },

    devicePaneItem: {
        margin: '0 -16px',
    },

    deviceLabel: {
        display: 'flex',
        alignItems: 'center',
        whiteSpace: 'nowrap',
        padding: '8px 0',
    },

    deviceIcon: {
        height: 16,
        width: 16,
        marginRight: 8,
        fill: 'currentColor',
    },

    noDevicesMessage: {
        whiteSpace: 'nowrap',
        margin: 8,
    },

    buttonLabel: {
        display: 'none',

        '@media (min-width: 720px)': {
            display: 'inline',
        },
    },

    buttonItem: {
        display: 'flex',
        flexDirection: 'row',
        alignItems: 'center',
        justifyContent: 'space-between',
    },

    buttonItemEditorPane: {
        margin: '-8px 12px 0 12px',
    },

    buttonItemDevicePane: {
        margin: -4,
    },

    title: {
        margin: '16px 0 8px',
    },

});
