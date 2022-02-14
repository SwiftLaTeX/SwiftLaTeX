import * as React from 'react';
import { StyleSheet, css } from 'aphrodite';

function AnimatedLogo() {

    return (
        <div className={css(styles.container)}>
            <div className={css(styles.loader)}>
            </div>
        </div>
    );
}

export default AnimatedLogo;

const keyframes = {
    a: {
        '0%': { transform: 'rotate(0deg)'},
        '100%': { transform: 'rotate(360deg)' },
    },
};

const styles = StyleSheet.create({
    container: {
        position: 'relative',
        overflow: 'hidden',
    },
    loader: {
        border: '16px solid #f3f3f3',
        borderTop: '16px solid #3498db',
        borderRadius: '50%',
        width: '120px',
        height: '120px',
        animationName: keyframes.a,
        animationDuration: '2s',
        animationIterationCount: 'infinite',
    }

});
