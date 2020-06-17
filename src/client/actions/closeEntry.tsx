import updateEntry from './updateEntry';
import { FileSystemEntry } from '../types';

export default function closeEntry(entry: FileSystemEntry) {
  if (entry.item.type === 'file') {
    if (entry.state.isSelected || entry.state.isFocused || entry.state.isOpen) {
      // Unselect and unfocus the file
      return updateEntry(entry, {
        state: {
          isOpen: false,
          isSelected: false,
          isFocused: false,
        },
      });
    }
  } else {
    if (entry.state.isSelected) {
      // Unselect the folder
      return updateEntry(entry, {
        state: {
          isSelected: false,
        },
      });
    }
  }
  return entry;
}
