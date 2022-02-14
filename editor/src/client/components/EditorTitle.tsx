import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';
import withThemeName, { ThemeName } from './Preferences/withThemeName';
import colors from '../configs/colors';
import { SaveStatus } from '../types';
import EditableField from './shared/EditableField';
import Helmet from 'react-helmet-async';

type Props = {
    name: string;
    saveStatus: SaveStatus;
    onSubmitTitle: (title: string) => Promise<void>;
    theme: ThemeName;
};

type State = {
    date: Date;
};

class EditorTitle extends React.Component<Props, State> {
    state = {
        date: new Date(),
    };

    componentDidMount() {
        this._timer = setInterval(() => this.setState({ date: new Date() }), 10000);
    }

    componentWillUnmount() {
        clearInterval(this._timer);
    }

    _timer: any;

    render() {
        const { name, saveStatus } = this.props;

        // const savedAt = createdAt;

        let statusText;

        // User is logged in
        if (saveStatus === 'changed') {
            statusText = 'Saving changes.';
        } else {
            statusText = 'Changes saved';
        }

        statusText = (
            <React.Fragment>
                <span className={css(styles.statusText)}>{statusText}.</span>
            </React.Fragment>
        );

        return (
            <div className={css(styles.container)}>
                <div className={css(styles.header)}>
                    <div className={css(styles.titleContainer)}>
                        <h1 className={css(styles.title)}>
                            <EditableField value={name} onSubmitText={this.props.onSubmitTitle} />
                        </h1>
                        <Helmet title={name} />
                    </div>
                    <div className={css(styles.metadata)}>
                        <p className={css(styles.status)}>{statusText}</p>
                        {saveStatus === 'changed' ? <div className={css(styles.spinner)} /> : null}
                        {saveStatus === 'published' ? (
                            <svg
                                className={css(styles.check)}
                                width="11px"
                                height="8px"
                                viewBox="0 0 11 8">
                                <polygon
                                    fill="#4CAF50"
                                    points="3.34328358 6.32835821 0.835820896 3.82089552 0 4.65671642 3.34328358 8 10.5074627 0.835820896 9.67164179 0"
                                />
                            </svg>
                        ) : null}
                    </div>
                </div>
            </div>
        );
    }
}

export default withThemeName(EditorTitle);

const spin = {
    from: { transform: 'rotate(0deg)' },
    to: { transform: 'rotate(360deg)' },
};

const styles = StyleSheet.create({
    container: {
        display: 'flex',
        alignItems: 'center',
        minWidth: 0,
        height: '100%',
    },

    header: {
        padding: 2,
        minWidth: 0,
    },

    title: {
        fontSize: '1.3em',
        lineHeight: '1.3em',
        fontWeight: 500,
        margin: 0,
        textOverflow: 'ellipsis',
        overflow: 'hidden',
    },

    titleContainer: {
        display: 'flex',
        flexDirection: 'row',
        alignItems: 'flex-end',
    },

    metadata: {
        display: 'flex',
        flexDirection: 'row',
        alignItems: 'center',
    },

    status: {
        fontSize: 12,
        margin: '0 6px',
        textOverflow: 'ellipsis',
        overflow: 'hidden',
    },

    textButton: {
        appearance: 'none',
        background: 'none',
        border: 0,
        margin: 0,
        padding: 0,
        textDecoration: 'underline',
    },

    statusText: {
        opacity: 0.5,
    },

    spinner: {
        borderStyle: 'solid',
        borderTopColor: colors.primary,
        borderLeftColor: colors.primary,
        borderBottomColor: colors.primary,
        borderRightColor: 'rgba(0, 0, 0, .16)',
        borderWidth: 1,
        height: 12,
        width: 12,
        borderRadius: '50%',
        margin: '0 4px',
        animationDuration: '1s',
        animationName: [spin],
        animationIterationCount: 'infinite',
        animationTimingFunction: 'linear',
    },

    check: {
        marginBottom: -4,
    },

    icon: {
        display: 'block',
        position: 'relative',
        appearance: 'none',
        backgroundColor: 'transparent',
        backgroundRepeat: 'no-repeat',
        backgroundSize: 16,
        backgroundPosition: 'center',
        border: 0,
        outline: 0,
        margin: 0,
        padding: 0,
        height: 24,
        width: 24,
        opacity: 0.3,
        transition: '.2s',

        ':hover': {
            opacity: 0.8,
        },
    },

    description: {
        margin: 16,
    },

    modal: {
        padding: '1em 1.5em',
        borderRadius: 3,
        width: 360,
        maxWidth: '100%',
        boxShadow: '0 1px 8px rgba(0, 0, 0, 0.07)',
    },

    infoLight: {
        backgroundImage: `url(${require('../assets/info-icon.png')})`,
    },

    infoDark: {
        backgroundImage: `url(${require('../assets/info-icon-light.png')})`,
    },

    editButton: {
        width: '100%',
        background: 'none',
        outline: 0,
        borderWidth: '1px 0 0 0',
        borderColor: colors.border,
        padding: '8px 16px',
    },
});
