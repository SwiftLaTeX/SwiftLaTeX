import * as React from 'react';
// import { connect } from 'react-redux';
import debounce from 'lodash/debounce';
import { parse } from 'query-string';
import { PreferencesContextType } from './withPreferences';
import { ThemeName } from './withThemeName';


export type PreferencesType = {
  deviceConnectionMethod: any;
  devicePreviewPlatform: string;
  devicePreviewShown: boolean;
  editorMode: 'normal' | 'vim';
  fileTreeShown: boolean;
  panelsShown: boolean;
  panelType: 'errors' | 'logs';
  theme: ThemeName;
};

export type SetPreferencesType = (overrides: Partial<PreferencesType>) => void;

type Props = {
  search: string;
  cookies: {
    get: (key: string) => string | undefined;
    set?: (key: string, value: string) => void;
  };
  testConnectionMethod?: any;
  testPreviewPlatform?: string;
  children: React.ReactNode;
};

type State = {
  preferences: PreferencesType;
};

const EDITOR_CONFIG_KEY = 'snack-editor-config';

const defaults: PreferencesType = {
  deviceConnectionMethod: 'device-id',
  devicePreviewPlatform: 'web',
  devicePreviewShown: true,
  editorMode: 'normal',
  fileTreeShown: true,
  panelsShown: false,
  panelType: 'errors',
  theme: 'light',
};

export const PreferencesContext = React.createContext<PreferencesContextType | null>(null);

class PreferencesProvider extends React.Component<Props, State> {
  constructor(props: Props) {
    super(props);

    const { cookies, search } = this.props;

    let overrides: Partial<PreferencesType> = {};

    try {
      // Restore editor preferences from saved data
      overrides = JSON.parse(cookies.get(EDITOR_CONFIG_KEY) || '') || {};
      overrides.devicePreviewPlatform = undefined;
    } catch (e) {
      // Ignore error
    }

    try {
      // Set theme if passed in query params
      const { theme, platform } = parse(search);

      if (theme === 'light' || theme === 'dark') {
        overrides.theme = theme;
      }

      if (platform === 'android' || platform === 'ios') {
        overrides.devicePreviewPlatform = platform;
      }
    } catch (e) {
      // Ignore error
    }

    this.state = {
      preferences: {
        ...defaults,
        ...overrides,

        // Set the values according to the priority: saved preference, test value, default value
        deviceConnectionMethod:
          overrides.deviceConnectionMethod ||
          this.props.testConnectionMethod ||
          defaults.deviceConnectionMethod,
        devicePreviewPlatform:
          overrides.devicePreviewPlatform ||
          this.props.testPreviewPlatform ||
          defaults.devicePreviewPlatform,
      },
    };
  }

  _persistPreferences = debounce(() => {
    const { cookies } = this.props;

    try {
      cookies.set && cookies.set(EDITOR_CONFIG_KEY, JSON.stringify(this.state.preferences));
    } catch (e) {
      // Ignore
    }
  }, 1000);

  _setPreferences = (overrides: Partial<PreferencesType>) => {
    this.setState(
      state => ({
        preferences: {
          ...state.preferences,
          ...overrides,
        },
      }),
      this._persistPreferences
    );
  };

  render() {
    return (
      <PreferencesContext.Provider
        value={{
          setPreferences: this._setPreferences,
          preferences: this.state.preferences,
        }}>
        {this.props.children}
      </PreferencesContext.Provider>
    );
  }
}

// export default connect((state: any) => ({
//   testConnectionMethod: state.splitTestSettings.defaultConnectionMethod,
// }))(PreferencesProvider);
export default PreferencesProvider;
