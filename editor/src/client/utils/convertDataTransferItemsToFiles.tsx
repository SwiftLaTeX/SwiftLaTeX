
export type WebkitDirectoryReaderResult = (WebkitFileEntry | WebkitDirectoryEntry)[];

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
    files: { file: File; path: string }[],
    path: string
) => {
    if (entry.isFile) {
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
    items: (File | WebkitFileEntry | WebkitDirectoryEntry)[],
    mappings: { [key: string]: string }
) {
    const files: { file: File; path: string }[] = [];
    await Promise.all(
        items.map((it) => {
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
