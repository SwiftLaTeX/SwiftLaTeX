import updateEntry from './updateEntry';
import createEntryAtPath from './createEntryAtPath';
import { isInsideFolder } from '../utils/fileUtilities';
import { FileSystemEntry } from '../types';
import { genRandomString } from '../utils/strings';

export default function createNewEntry(
    entries: FileSystemEntry[],
    type: 'file' | 'folder',
    at?: string | undefined,
) {
    const e: FileSystemEntry =
        type === 'file'
            ? {
                item: {
                    path: 'Untitled.tex',
                    type: 'file',
                    content: '',
                    uri: '',
                    asset: false,
                    id: genRandomString() + '.tex',
                },
                state: {
                    isCreating: true,
                },
            }
            : {
                item: {
                    path: 'UntitledFolder',
                    type: 'folder',
                    content: '',
                    uri: '',
                    asset: false,
                    id: genRandomString()
                },
                state: {
                    isCreating: true,
                },
            };

    let path = at;

    if (typeof path !== 'string') {
        // Get the current folder if no path is specified
        const entry = entries.find(e => e.state.isSelected === true);

        path = entry ? entry.item.path : undefined;
    }

    const entry = createEntryAtPath(entries, path, e);

    const next = entries.map(e => {
        // Expand the parent folder
        if (
            e.item.type === 'folder' &&
            isInsideFolder(entry.item.path, e.item.path) &&
            !e.state.isExpanded
        ) {
            return updateEntry(e, {
                state: { isExpanded: true },
            });
        }

        return e;
    });

    return [...next, entry];
}
