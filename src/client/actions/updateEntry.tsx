import { FileSystemEntry } from '../types';

export default function updateEntry<T extends FileSystemEntry>(
  entry: T,
  updates: { item?: Partial<T['item']>; state?: Partial<T['state']> }
): T {
  return {
    ...(entry as any),
    item: updates.item ? { ...entry.item, ...(updates as any).item } : entry.item,
    state: updates.state ? { ...entry.state, ...(updates as any).state } : entry.state,
  };
}
