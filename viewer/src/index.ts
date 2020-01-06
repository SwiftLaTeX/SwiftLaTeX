
import { Machine } from "./machine";

import { parseDVI } from "./parser";

export function dvi2html(buf: Uint8Array) {
  	let machine = new Machine();
  	parseDVI(buf, machine);
  	return machine.output;
}

