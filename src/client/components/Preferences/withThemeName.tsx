import * as React from 'react';
import hoistNonReactStatics from 'hoist-non-react-statics';
import { PreferencesContext } from './PreferencesProvider';
import { $Subtract } from '../../types';

export type ThemeName = 'light' | 'dark';

type InjectedProps = {
    theme: ThemeName;
};

// react-redux doesn't work with forwardRef: https://github.com/reduxjs/react-redux/issues/914
// so this HOC always needs wrap a connect call, and a connect call cannot wrap this
export default function withThemeName<P extends InjectedProps>(
    Comp: React.ComponentType<P>
): React.ComponentType<$Subtract<P, InjectedProps>> {
    class ThemedComponent extends React.Component<$Subtract<P, InjectedProps>> {
        static displayName = `withTheme(${Comp.displayName || Comp.name})`;

        render() {
            // @ts-ignore
            const { __forwardedRef, ...rest } = this.props;

            return (
                <PreferencesContext.Consumer>
                    {(props) => {
                        // @ts-ignore
                        return (
                            <Comp ref={__forwardedRef} theme={props.preferences.theme} {...rest} />
                        );
                    }}
                </PreferencesContext.Consumer>
            );
        }
    }

    const Result = React.forwardRef((props, ref) => (
        // @ts-ignore
        <ThemedComponent {...props} __forwardedRef={ref} />
    ));

    hoistNonReactStatics(Result, Comp);

    return Result as any;
}
