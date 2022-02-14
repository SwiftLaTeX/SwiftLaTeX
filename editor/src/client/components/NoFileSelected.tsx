import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';

export default class NoFileSelected extends React.PureComponent {
    render() {
        return (
            <div className={css(styles.container)}>
                <h4 className={css(styles.text)}>Select a file to view and edit</h4>
            </div>
        );
    }
}

const styles = StyleSheet.create({
    container: {
        flex: 1,
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'center',
    },
    text: {
        fontSize: '1.5em',
        fontWeight: 500,
        opacity: 0.5,
    },
});
