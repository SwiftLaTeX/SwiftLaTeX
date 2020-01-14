'use es6';
import React from 'react';

import 'codemirror/lib/codemirror.css';
import 'codemirror/theme/material.css';
// assuming a setup with webpack/create-react-app import the additional js/css files
import 'codemirror/addon/hint/show-hint';
import 'codemirror/addon/hint/anyword-hint';
import 'codemirror/addon/hint/show-hint.css'; // without this css hints won't show
import {UnControlled as CodeMirror} from 'react-codemirror2'

export default class Editor extends React.Component {
    constructor(prop) {
        super(prop);
        this.instance = null;
    }

    componentDidMount() {
        /*Size*/
        this.instance.setSize("100%", "400px");
    }

    render() {
        return (
            <CodeMirror
                value='<h1>I ♥ react-codemirror2</h1>'
                options={{
                    mode: 'xml',
                    autoCursor: false,
                    theme: 'material',
                    lineNumbers: true,
                    extraKeys: {
                        "Tab": "autocomplete"
                    },
                    hintOptions: {
                        completeSingle: false,
                    }
                }}
                editorDidMount={editor => { this.instance = editor }}
                onKeyUp={(cm, event) => {

                    if (!cm.state.completionActive && /*Enables keyboard navigation in autocomplete list*/
                        event.keyCode >=48 && event.keyCode <= 90) {
                        console.log(event);
                        /*Enter - do not open autocomplete list just after item has been selected in it*/
                        cm.execCommand("autocomplete");
                    }
                }}
            />
        );
    }
}