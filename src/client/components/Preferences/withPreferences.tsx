import * as React from 'react';
import hoistNonReactStatics from 'hoist-non-react-statics';
import { PreferencesContext, PreferencesType, SetPreferencesType } from './PreferencesProvider';
import { $Subtract } from '../../types';

export type PreferencesContextType = {
  setPreferences: SetPreferencesType;
  preferences: PreferencesType;
};

// react-redux doesn't work with forwardRef: https://github.com/reduxjs/react-redux/issues/914
// so this HOC always needs wrap a connect call, and a connect call cannot wrap this
export default function withPreferences<P extends PreferencesContextType>(
  Comp: React.ComponentType<P>
): React.ComponentType<$Subtract<P, PreferencesContextType>> {
  class PreferenceConsumerComponent extends React.Component<$Subtract<P, PreferencesContextType>> {
    static displayName = `withPreferences(${Comp.displayName || Comp.name})`;

    render() {
      // @ts-ignore
      const { forwardedRef, ...rest } = this.props;

      return (
        <PreferencesContext.Consumer>
          {props => {
            // @ts-ignore
            return <Comp ref={forwardedRef} {...props} {...rest} />;
          }}
        </PreferencesContext.Consumer>
      );
    }
  }

  /* $FlowIssue: Flow doesn't know about forwardRef yet */
  const Result = React.forwardRef((props, ref) => (
    // @ts-ignore
    <PreferenceConsumerComponent {...props} forwardedRef={ref} />
  ));

  hoistNonReactStatics(Result, Comp);

  return Result as any;
}
