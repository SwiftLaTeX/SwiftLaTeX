import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';
import withThemeName, { ThemeName } from '../Preferences/withThemeName';
import colors from '../../configs/colors';

type Props = {
    theme: ThemeName;
    value: string | undefined;
    disabled?: boolean;
    onChange: (event: React.ChangeEvent<HTMLInputElement>) => void;
    placeholder?: string;
    name?: string;
    type?: string;
    autoFocus?: boolean;
    error?: Error | null;
};

type State = {
    focused: boolean;
};

class LargeInput extends React.Component<Props, State> {
    state = {
        focused: true,
    };

    focus() {
        this._input.current && this._input.current.focus();
    }

    _handleFocus = () => this.setState({ focused: true });

    _handleBlur = () => this.setState({ focused: false });

    _input = React.createRef<HTMLInputElement>();

    render() {
        return (
            <div className={css(styles.container)}>
                <input
                    ref={this._input}
                    autoFocus={this.props.autoFocus}
                    className={css(
                        styles.input,
                        this.props.error ? styles.error : styles.normal,
                        this.props.theme === 'dark' ? styles.inputDark : styles.inputLight
                    )}
                    value={this.props.value}
                    name={this.props.name}
                    type={this.props.type}
                    disabled={this.props.disabled}
                    onChange={this.props.onChange}
                    placeholder={this.props.placeholder}
                    onFocus={this._handleFocus}
                    onBlur={this._handleBlur}
                />
                {this.state.focused && this.props.error ? (
                    <div className={css(styles.validation)}>{this.props.error.message}</div>
                ) : null}
            </div>
        );
    }
}

export default withThemeName(LargeInput);

const styles = StyleSheet.create({
    container: {
        position: 'relative',
    },
    input: {
        outline: 0,
        fontSize: 16,
        borderRadius: 3,
        padding: '12px 14px 12px 14px',
        lineHeight: '22px',
        width: '100%',
        borderWidth: 1,
        borderStyle: 'solid',
    },
    normal: {
        borderColor: 'rgba(36, 44, 58, 0.1)',

        ':focus': {
            borderColor: colors.primary,
        },
    },
    error: {
        borderColor: colors.error,
    },
    inputLight: {
        backgroundColor: '#FFFFFF',
    },
    inputDark: {
        backgroundColor: 'rgba(0, 0, 0, .16)',
    },
    validation: {
        position: 'absolute',
        backgroundColor: colors.error,
        color: 'white',
        padding: '6px 12px',
        borderRadius: 3,
        marginTop: 8,
        textAlign: 'left',
        zIndex: 1,

        ':before': {
            content: '""',
            display: 'block',
            position: 'absolute',
            bottom: '100%',
            width: 0,
            height: 0,
            borderLeft: '6px solid transparent',
            borderRight: '6px solid transparent',
            borderBottom: `6px solid ${colors.error}`,
        },
    },
});
