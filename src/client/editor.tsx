import * as React from 'react';
import ReactDOM from 'react-dom';
import { BrowserRouter } from 'react-router-dom';
import cookies from 'js-cookie';
import PreferencesProvider from './components/Preferences/PreferencesProvider';
import ColorsProvider from './components/ColorsProvider';
import App from './components/App';
import { HelmetProvider } from 'react-helmet-async';


class SnackEntry extends React.Component {
  render() {
    return (
      <React.StrictMode>
        <HelmetProvider>
          <PreferencesProvider cookies={cookies} search={window.location.search}>
            <ColorsProvider>
              <BrowserRouter>
                <App {... this.props}/>
              </BrowserRouter>
            </ColorsProvider>
          </PreferencesProvider>
        </HelmetProvider>
      </React.StrictMode>
    );
  }
}

ReactDOM.render(<SnackEntry />, document.getElementById('root'));
