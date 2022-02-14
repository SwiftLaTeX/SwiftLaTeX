import { genFileID, getParentPath } from '../utils/fileUtilities';
import { FileManagerEntry } from '../types';

export default function recursivelyCreateParents(
    entries: FileManagerEntry[],
    path: string,
    expand?: boolean
): FileManagerEntry[] {
    const next: FileManagerEntry[] = [];

    let parent = getParentPath(path);

    while (parent) {
        const parentEntry = entries.find((e) => e.item.path === parent);

        if (parentEntry) {
            if (parentEntry.item.type !== 'folder') {
                throw new Error('File path must be inside a folder');
            }
            break;
        } else {
            next.push({
                item: {
                    type: 'folder',
                    path: parent,
                    asset: false,
                    uri: '',
                    content: '',
                    id: genFileID(parent),
                },
                state: {
                    isExpanded: !!expand,
                },
            });

            parent = getParentPath(parent);
        }
    }

    return next;
}
