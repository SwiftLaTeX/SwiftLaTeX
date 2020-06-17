import updateEntry from './updateEntry';
import { FileSystemEntry } from '../types';

export default function openEntry(
  entries: FileSystemEntry[],
  path: string,
  focus: boolean = false
): FileSystemEntry[] {
  const entry = entries.find(e => e.item.path === path);
  const isFolder = entry ? entry.item.type === 'folder' : false;

  return entries.map(e => {
    if (e.item.path === path) {
      // Found the file/folder we're trying to select
      if (e.item.type === 'file') {
        if (e.state.isSelected && e.state.isFocused && e.state.isOpen) {
          // File is already selected, opened and focused
          // Nothing to do
          return e;
        }

        // Select, open and focus the file
        return updateEntry(e, {
          state: {
            isSelected: true,
            isFocused: true,
            isOpen: true,
          },
        });
      } else {
        // Select and toggle the expand for the directory
        return updateEntry(e, {
          state: {
            isSelected: true,

            isExpanded: focus ? e.state.isExpanded : !e.state.isExpanded,
          },
        });
      }
    }

    // Not the file we're trying to select
    if (e.item.type === 'file') {
      if (e.state.isSelected || e.state.isFocused) {
        // Unselect and unfocus the file
        return updateEntry(e, {
          state: {
            isSelected: false,

            isFocused: isFolder ? e.state.isFocused : false,
          },
        });
      }
    } else {
      if (e.state.isSelected) {
        // Unselect the folder

        return updateEntry(e, {
          state: {
            isSelected: false,
          },
        });
      }
    }

    return e;
  });
}
