declare module 'json-to-ast' {
  export default function parse(code: string, options?: { source?: string }): any;
}
