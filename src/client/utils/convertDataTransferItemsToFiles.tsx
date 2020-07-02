import { isImageFile, isOpenTypeFontFile, isTextFile } from './fileUtilities';

export type WebkitDirectoryReaderResult = Array<WebkitFileEntry | WebkitDirectoryEntry>;

export type WebkitDirectoryReader = {
  readEntries: (
    success: (result: WebkitDirectoryReaderResult) => void,
    error: (e: Error) => void
  ) => void;
};

export type WebkitFileEntry = {
  isFile: true;
  isDirectory: false;
  name: string;
  fullPath: string;
  file: (success: (f: File) => void, failure: (e: Error) => void) => void;
};

export type WebkitDirectoryEntry = {
  isFile: false;
  isDirectory: true;
  name: string;
  fullPath: string;
  createReader: () => WebkitDirectoryReader;
};



const processEntry = async (
  entry: WebkitFileEntry | WebkitDirectoryEntry,
  files: Array<{ file: File; path: string }>,
  path: string
) => {



  if (entry.isFile) {
    if (!isTextFile(entry.name) && !isImageFile(entry.name) && !isOpenTypeFontFile(entry.name)) {
        return;
    }
    const file = await new Promise<File>((resolve, reject) => entry.file(resolve, reject));
    files.push({
      path,
      file,
    });
  } else if (entry.isDirectory) {
    const reader = entry.createReader();
    const entries = await new Promise<WebkitDirectoryReaderResult>((resolve, reject) =>
      reader.readEntries(resolve, reject)
    );
    await Promise.all(
      entries.map(async (e: WebkitFileEntry | WebkitDirectoryEntry) =>
        processEntry(e, files, `${path}/${e.name}`)
      )
    );
  }
};

export default async function convertDataTransferItemsToFiles(
  items: Array<File | WebkitFileEntry | WebkitDirectoryEntry>,
  mappings: { [key: string]: string }
) {
  const files: Array<{ file: File; path: string }> = [];
  await Promise.all(
    items.map(it => {
      const path = mappings[it.name] || it.name;
      if (it instanceof File) {
        files.push({
          file: it,
          path,
        });

        return undefined;
      } else {
        return processEntry(it, files, path);
      }
    })
  );
  return files;
}
