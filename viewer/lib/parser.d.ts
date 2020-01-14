import { Machine } from "./machine";
export declare class DviCommand {
    length: number;
    special: boolean;
    constructor(properties: any);
    execute(machine: Machine): void;
    toString(): string;
}
export declare function parseDVI(dviContent: Uint8Array, machine: Machine): void;
