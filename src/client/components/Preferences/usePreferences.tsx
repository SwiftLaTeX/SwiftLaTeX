import { useContext } from 'react';
import { PreferencesContext, PreferencesType, SetPreferencesType } from './PreferencesProvider';
import { PreferencesContextType } from './withPreferences';

export default function usePreferences(): [PreferencesType, SetPreferencesType] {
    const { preferences, setPreferences } = useContext(
        PreferencesContext
    ) as PreferencesContextType;

    return [preferences, setPreferences];
}
