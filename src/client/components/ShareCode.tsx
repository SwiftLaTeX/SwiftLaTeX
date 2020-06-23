import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';
import CopyToClipboard from 'react-copy-to-clipboard';
import Banner from './shared/Banner';
import Button from './shared/Button';
import colors from '../configs/colors';

import { ThemeName } from './Preferences/withThemeName';

const handleClick = (e: any) => e.target.select();

type Props = {
    params?: {
        id?: string;
    };
    theme: ThemeName;
};

type State = {
    theme: 'light' | 'dark';
    copied: boolean;
};

class ShareCode extends React.PureComponent<Props, State> {
    constructor(props: Props) {
        super(props);

        this.state = {
            theme: this.props.theme || 'light',
            copied: false,
        };
    }

    state: State;

    componentDidMount() {

    }

    componentDidUpdate(_: Props, __: State) {

    }

    _handleCopy = () => {
        this.setState({ copied: true });
        setTimeout(() => this.setState({ copied: false }), 1000);
    };


    render() {
        const { theme, copied } = this.state;

        const code = `We are working hard to make sharing function work`;

        return (
            <div className={css(styles.container)}>
                <Banner visible={copied}>Copied to clipboard!</Banner>
                <div className={css(styles.section)}>
                    <h3 className={css(styles.header)}>Share1 Code</h3>
                    <div className={css(styles.inputContainer)}>
                        <input
                            readOnly
                            className={css(styles.code, theme === 'light' ? styles.inputLight : styles.inputDark)}
                            onClick={handleClick}
                            value={code}
                        />
                        <CopyToClipboard text={code} onCopy={this._handleCopy}>
                            <Button variant="primary" className={css(styles.copyButton)}>
                                Copy to clipboard
                            </Button>
                        </CopyToClipboard>
                    </div>
                </div>
            </div>
        );
    }
}

export default ShareCode;

const styles = StyleSheet.create({
    container: {
        width: 780,
        textAlign: 'left',
    },
    header: {
        margin: '.5em 0',
        display: 'inline',
        fontWeight: 500,
    },
    options: {
        color: '#999',
        marginBottom: '.5em',
    },
    last: {
        marginRight: 0,
    },
    row: {
        display: 'flex',
        flexDirection: 'row',
        alignItems: 'center',
    },
    section: {
        ':not(:last-of-type)': {
            paddingBottom: '1.5em',
        },
    },
    code: {
        fontFamily: 'var(--font-monospace)',
        padding: '1em',
        width: '100%',
        outline: 0,
        border: `1px solid ${colors.border}`,
        borderRadius: 3,
    },
    inputLight: {
        backgroundColor: colors.background.light,
        color: colors.text.light,
    },
    inputDark: {
        backgroundColor: 'rgba(0, 0, 0, .2)',
        color: colors.text.dark,
    },
    inputContainer: {
        position: 'relative',
    },
    copyButton: {
        position: 'absolute',
        right: 0,
    },
    textButton: {
        appearance: 'none',
        background: 'none',
        border: 0,
        margin: 0,
        padding: '0px 4px',
        textDecoration: 'underline',
    },
});