import updateEntry from './updateEntry';
import { FileManagerEntry } from '../types';

export default function expandEntry(
    entries: FileManagerEntry[],
    path: string,
    expand: boolean = true
): FileManagerEntry[] {
    return entries.map((entry) => {
        if (entry.item.path === path && entry.item.type === 'folder') {
            return updateEntry(entry, {
                state: {
                    isExpanded: expand,
                },
            });
        }

        return entry;
    });
}
