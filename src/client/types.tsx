
export type Annotation = {
  startLineNumber: number;
  endLineNumber: number;
  startColumn: number;
  endColumn: number;
  message: string;
  severity: number;
  source: string;
};

export type FileSystemEntry = {
  item: {
    path: string;
    type: 'file' | 'folder';
    uri: string;
    content: string | ArrayBuffer;
    asset: boolean;
    id: string;
  };
  state: {
    isOpen?: boolean;
    isFocused?: boolean;
    isSelected?: boolean;
    isCreating?: boolean;
    isExpanded?: boolean;
  };
};


export type SaveStatus = 'changed' | 'saving-draft' | 'saved-draft' | 'publishing' | 'published';

export type $SetComplement<A, A1 extends A> = A extends A1 ? never : A;

export type $Subtract<T extends T1, T1 extends object> = Pick<T, $SetComplement<keyof T, keyof T1>>;
