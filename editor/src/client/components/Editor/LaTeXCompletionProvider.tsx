import * as monaco from 'monaco-editor/esm/vs/editor/editor.api';

const ENV_REGEXP = /(?:\\begin(?:\[[^[\]]*\])?){([^}]*)$/;

const CMD_REGEXP = /\\([a-zA-Z_]*(?::[a-zA-Z]*)?)$/;

const PACKAGE_REGEXP = /(?:\\usepackage(?:\[[^[\]]*\])*){([^}]*)$/;

const DOCUMENTCLASS_REGEXP = /(?:\\documentclass(?:\[[^[\]]*\])*){([^}]*)$/;


function unpackJson(jsonInput: any, suggestionOutput: monaco.languages.CompletionItem[]) {
    Object.keys(jsonInput).forEach(key => {
        const item = jsonInput[key];
        const pack: monaco.languages.CompletionItem = {
            kind: monaco.languages.CompletionItemKind.Module,
            detail: item.detail ? item.detail : '',
            documentation: item.documentation ? item.documentation : '',
            label: item.command,
            insertText: item.command,
        } as monaco.languages.CompletionItem;
        suggestionOutput!.push(pack);
    });
}

let documentclassSuggestion: monaco.languages.CompletionItem[] | undefined;

async function suggestDocumentClass() {
    if (!documentclassSuggestion) {
        const freq = await fetch('/bin/lang/classnames.json');
        if (freq.ok) {
            const classes = await freq.json();
            documentclassSuggestion = [];
            unpackJson(classes, documentclassSuggestion);
        } else {
            documentclassSuggestion = [];
        }
    }
    return documentclassSuggestion;
}


let packageSuggestion: monaco.languages.CompletionItem[] | undefined;

async function suggestPackage() {
    if (!packageSuggestion) {
        const freq = await fetch('/bin/lang/packagenames.json');
        if (freq.ok) {
            const defaultPackages = await freq.json();
            packageSuggestion = [];
            unpackJson(defaultPackages, packageSuggestion);
        } else {
            packageSuggestion = [];
        }
    }
    return packageSuggestion;
}

let envSuggestion: monaco.languages.CompletionItem[] | undefined;

async function suggestEnv() {
    if (!envSuggestion) {
        const freq = await fetch('/bin/lang/environments.json');
        if (freq.ok) {
            const inputJson = await freq.json();
            envSuggestion = [];
            unpackJson(inputJson, envSuggestion);
        } else {
            envSuggestion = [];
        }
    }
    return envSuggestion;
}

let cmdSuggestion: monaco.languages.CompletionItem[] | undefined;

async function suggestCMD() {
    if (!cmdSuggestion) {
        const freq = await fetch('/bin/lang/commands.json');
        if (freq.ok) {
            const inputJson = await freq.json();
            cmdSuggestion = [];
            unpackJson(inputJson, cmdSuggestion);
        } else {
            cmdSuggestion = [];
        }
    }
    return cmdSuggestion;
}

async function suggest(lineBeforeInvokeChar: string): Promise<monaco.languages.CompletionList> {
    let items: monaco.languages.CompletionItem[] = [];

    if (lineBeforeInvokeChar.match(CMD_REGEXP)) {
        const tmp = await suggestCMD();
        items = items.concat(tmp);
    }

    if (lineBeforeInvokeChar.match(ENV_REGEXP)) {
        const tmp = await suggestEnv();
        items = items.concat(tmp);
    }

    if (lineBeforeInvokeChar.match(PACKAGE_REGEXP)) {
        const tmp = await suggestPackage();
        items = items.concat(tmp);
    }

    if (lineBeforeInvokeChar.match(DOCUMENTCLASS_REGEXP)) {
        const tmp = await suggestDocumentClass();
        items = items.concat(tmp);
    }

    const result: monaco.languages.CompletionList = {
        suggestions: items,
    };
    return result;
}

export const laTeXCompletionProvider: monaco.languages.CompletionItemProvider = {
    triggerCharacters: ['\\', '{'],
    provideCompletionItems: (model: monaco.editor.ITextModel, position: monaco.Position) => {

        const currentLine = model.getLineContent(position.lineNumber);
        const invokeChar = currentLine[position.column - 1];
        if (position.column > 1 && invokeChar === '\\' && currentLine[position.column - 2] === '\\') {
            return undefined;
        }
        let startPos = position.column - 32; /* Should be enough */
        if (startPos < 0) {
            startPos = 0;
        }
        const lineBeforeInvokeChar = currentLine.substr(startPos, position.column - 1);

        return suggest(lineBeforeInvokeChar);
    },
};