import React from 'react';
import './App.css';
import './split.css'
import 'semantic-ui-css/semantic.min.css'
import NavMenu from "./Nav"
import FilePanel from "./FilePanel"
import SplitPane from "react-split-pane"
import Editor from "./Editor"
class App extends React.Component {
    render(){
        return (
            <div className="App">
                    <NavMenu/>
                    <SplitPane split="vertical" defaultSize="200px" className="Workspace">
                            <FilePanel/>
                            <SplitPane defaultSize="50%">
                                <Editor/>
                                <div/>
                            </SplitPane>
                    </SplitPane>

            </div>
        );
    }
}

export default App;
