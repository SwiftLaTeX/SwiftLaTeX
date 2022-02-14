import updateEntry from './updateEntry';
import recursivelyCreateParents from './recursivelyCreateParents';
import { changeParentPath, isInsideFolder } from '../utils/fileUtilities';
import { FileManagerEntry } from '../types';

export default function renameEntry(entries: FileManagerEntry[], oldPath: string, newPath: string) {
    const entry = entries.find((e) => e.item.path === oldPath);

    if (!entry) {
        return entries;
    }

    newPath = newPath.replace(/\s/g, '_');

    const renamed = updateEntry(entry, {
        item: {
            path: newPath,
        },
    });

    // @ts-ignore
    delete renamed.state.isCreating;

    const next: FileManagerEntry[] = entries.map((e) => {
        if (e.item.path === entry.item.path) {
            return renamed;
        }

        if (renamed.item.type === 'folder' && isInsideFolder(e.item.path, entry.item.path)) {
            return updateEntry(e, {
                item: {
                    path: changeParentPath(e.item.path, entry.item.path, renamed.item.path),
                },
            });
        }

        return e;
    });

    const parents = recursivelyCreateParents(next, renamed.item.path, true);

    return [...next, ...parents];
}
