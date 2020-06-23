declare module 'jsesc' {
  export default function jsesc(
    value: any,
    options: {
      quotes?: 'single' | 'double' | 'backtick';
      numbers?: 'decimal' | 'binary' | 'octal' | 'hexadecimal';
      wrap?: boolean;
      es6?: boolean;
      escapeEverything?: boolean;
      minimal?: boolean;
      isScriptContext?: boolean;
      compact?: boolean;
      indent?: string;
      indentLevel?: number;
      json?: boolean;
      lowercaseHex?: boolean;
    }
  ): string;
}
