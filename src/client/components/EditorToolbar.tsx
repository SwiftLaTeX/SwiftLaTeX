import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';
import ToolbarShell from './Shell/ToolbarShell';
import ToolbarTitleShell from './Shell/ToolbarTitleShell';
import IconButton from './shared/IconButton';
import EditorTitle from './EditorTitle';


import { SaveStatus } from '../types';
import usePreferences from './Preferences/usePreferences';

type Props = {
    name: string;
    saveStatus: SaveStatus;
    isSystemBusy: boolean;
    onExportPDF: () => Promise<void>;
    onDownloadCode: () => Promise<void>;
    onSubmitTitle: (title: string) => Promise<void>;
};

export default function EditorToolbar(props: Props) {
    const [preferences] = usePreferences();

    const {
        name,
        saveStatus,
        onExportPDF,
        onDownloadCode,
        isSystemBusy,
        onSubmitTitle,
    } = props;
    const { theme } = preferences;

    const onExit = () => {
        window.location.href = '/project.html';
    };

    return (
        <ToolbarShell>
            <ToolbarTitleShell>
                <img
                    src={
                        theme === 'dark'
                            ? require('../assets/snack-icon-dark.svg')
                            : require('../assets/snack-icon.svg')
                    }
                    alt="Snack"
                    className={css(styles.logo)}
                />

                <EditorTitle
                    name={name}
                    saveStatus={saveStatus}
                    onSubmitTitle={onSubmitTitle}
                />
            </ToolbarTitleShell>
            <div className={css(styles.buttons)}>
                <IconButton
                    responsive
                    title="Download Source Codes"
                    label="Share"
                    disabled={isSystemBusy}
                    onClick={onDownloadCode}
                >
                    <svg width="24px" height="16px" viewBox="0 0 20 18">
                        <path fillRule="evenodd"
                            d="M14.68,12.621c-0.9,0-1.702,0.43-2.216,1.09l-4.549-2.637c0.284-0.691,0.284-1.457,0-2.146l4.549-2.638c0.514,0.661,1.315,1.09,2.216,1.09c1.549,0,2.809-1.26,2.809-2.808c0-1.548-1.26-2.809-2.809-2.809c-1.548,0-2.808,1.26-2.808,2.809c0,0.38,0.076,0.741,0.214,1.073l-4.55,2.638c-0.515-0.661-1.316-1.09-2.217-1.09c-1.548,0-2.808,1.26-2.808,2.809s1.26,2.808,2.808,2.808c0.9,0,1.702-0.43,2.217-1.09l4.55,2.637c-0.138,0.332-0.214,0.693-0.214,1.074c0,1.549,1.26,2.809,2.808,2.809c1.549,0,2.809-1.26,2.809-2.809S16.229,12.621,14.68,12.621M14.68,2.512c1.136,0,2.06,0.923,2.06,2.06S15.815,6.63,14.68,6.63s-2.059-0.923-2.059-2.059S13.544,2.512,14.68,2.512M5.319,12.061c-1.136,0-2.06-0.924-2.06-2.06s0.923-2.059,2.06-2.059c1.135,0,2.06,0.923,2.06,2.059S6.454,12.061,5.319,12.061M14.68,17.488c-1.136,0-2.059-0.922-2.059-2.059s0.923-2.061,2.059-2.061s2.06,0.924,2.06,2.061S15.815,17.488,14.68,17.488"/>
                    </svg>
                </IconButton>
                <IconButton
                    responsive
                    title="Download Source Codes"
                    label="Download"
                    disabled={isSystemBusy}
                    onClick={onDownloadCode}
                >
                    <svg width="24px" height="16px" viewBox="0 0 20 18">
                        <path
                            fillRule="evenodd"
                            d="M17.896,12.706v-0.005v-0.003L15.855,2.507c-0.046-0.24-0.255-0.413-0.5-0.413H4.899c-0.24,0-0.447,0.166-0.498,0.4L2.106,12.696c-0.008,0.035-0.013,0.071-0.013,0.107v4.593c0,0.28,0.229,0.51,0.51,0.51h14.792c0.28,0,0.51-0.229,0.51-0.51v-4.593C17.906,12.77,17.904,12.737,17.896,12.706 M5.31,3.114h9.625l1.842,9.179h-4.481c-0.28,0-0.51,0.229-0.51,0.511c0,0.703-1.081,1.546-1.785,1.546c-0.704,0-1.785-0.843-1.785-1.546c0-0.281-0.229-0.511-0.51-0.511H3.239L5.31,3.114zM16.886,16.886H3.114v-3.572H7.25c0.235,1.021,1.658,2.032,2.75,2.032c1.092,0,2.515-1.012,2.749-2.032h4.137V16.886z"
                        />
                    </svg>
                </IconButton>
                <IconButton responsive title="Save as PDF" label="Export PDF" onClick={onExportPDF}
                            disabled={isSystemBusy}>
                    <svg width="20px" height="18px" viewBox="0 0 20 18">
                        <path
                            fillRule="evenodd"
                            d="M17.064,4.656l-2.05-2.035C14.936,2.544,14.831,2.5,14.721,2.5H3.854c-0.229,0-0.417,0.188-0.417,0.417v14.167c0,0.229,0.188,0.417,0.417,0.417h12.917c0.229,0,0.416-0.188,0.416-0.417V4.952C17.188,4.84,17.144,4.733,17.064,4.656M6.354,3.333h7.917V10H6.354V3.333z M16.354,16.667H4.271V3.333h1.25v7.083c0,0.229,0.188,0.417,0.417,0.417h8.75c0.229,0,0.416-0.188,0.416-0.417V3.886l1.25,1.239V16.667z M13.402,4.688v3.958c0,0.229-0.186,0.417-0.417,0.417c-0.229,0-0.417-0.188-0.417-0.417V4.688c0-0.229,0.188-0.417,0.417-0.417C13.217,4.271,13.402,4.458,13.402,4.688"/>
                    </svg>
                </IconButton>
                <IconButton responsive title="Go back to project management." label="Home" onClick={onExit}
                            disabled={isSystemBusy}>
                    <svg width="20px" height="18px" viewBox="0 0 20 18">
                        <path fillRule="evenodd"
                              d="M18.121,9.88l-7.832-7.836c-0.155-0.158-0.428-0.155-0.584,0L1.842,9.913c-0.262,0.263-0.073,0.705,0.292,0.705h2.069v7.042c0,0.227,0.187,0.414,0.414,0.414h3.725c0.228,0,0.414-0.188,0.414-0.414v-3.313h2.483v3.313c0,0.227,0.187,0.414,0.413,0.414h3.726c0.229,0,0.414-0.188,0.414-0.414v-7.042h2.068h0.004C18.331,10.617,18.389,10.146,18.121,9.88 M14.963,17.245h-2.896v-3.313c0-0.229-0.186-0.415-0.414-0.415H8.342c-0.228,0-0.414,0.187-0.414,0.415v3.313H5.032v-6.628h9.931V17.245z M3.133,9.79l6.864-6.868l6.867,6.868H3.133z"/>
                    </svg>
                </IconButton>

            </div>
        </ToolbarShell>
    );
}

const styles = StyleSheet.create({
    logo: {
        width: 36,
        height: 'auto',
        margin: '0 .5em 0 .75em',
    },

    buttons: {
        display: 'flex',
        flexDirection: 'row',
        alignItems: 'center',
        position: 'relative',
        zIndex: 5,
    },

    saveButton: {
        minWidth: 100,
    },
});
