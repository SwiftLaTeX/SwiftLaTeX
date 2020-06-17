import createEntryAtPath from './createEntryAtPath';
import updateEntry from './updateEntry';
import { isInsideFolder, changeParentPath } from '../utils/fileUtilities';
import { FileSystemEntry } from '../types';

export default function handleEntryPaste(
  entries: FileSystemEntry[],
  path: string | undefined,
  e: FileSystemEntry
) {
  const entry = createEntryAtPath(entries, path, e, '_copy');

  if (e.item.type === 'folder') {
    const children = entries
      .filter(it =>
        // Get children of the old folder
        isInsideFolder(it.item.path, e.item.path)
      )
      .map(it =>
        // Update the parent folder name to the new one
        updateEntry(it, {
          item: {
            path: changeParentPath(it.item.path, e.item.path, entry.item.path),
          },
        })
      );

    return [...entries, ...children, entry];
  } else {
    return [...entries, entry];
  }
}
