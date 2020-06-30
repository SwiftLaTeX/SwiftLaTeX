import { getParentPath } from '../utils/fileUtilities';
import { FileSystemEntry } from '../types';

export default function recursivelyCreateParents(
  entries: FileSystemEntry[],
  path: string,
  expand?: boolean
): FileSystemEntry[] {
  const next: FileSystemEntry[] = [];

  let parent = getParentPath(path);

  while (parent) {
    const parentEntry = entries.find(e => e.item.path === parent);

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
          id: '',
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
