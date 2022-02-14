import updateEntry from './updateEntry';
import { FileManagerEntry } from '../types';

export default function selectEntry(entries: FileManagerEntry[], path: string): FileManagerEntry[] {
    return entries.map((e) =>
        e.item.path === path
            ? updateEntry(e, {
                  state: {
                      isSelected: !e.state.isSelected,
                  },
              })
            : e
    );
}
