declare module 'recast' {
  type PrintResult = { code: string };

  export function print(ast: any): PrintResult;

  export function parse(
    code: string,
    options: {
      parser: { parse: (code: string) => any };
    }
  ): PrintResult;

  export const types: {
    visit(
      ast: any,
      visitor: {
        [key: string]: (path: any) => void;
      }
    ): void;
  };
}
