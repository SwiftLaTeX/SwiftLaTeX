import { DviCommand, merge } from '../parser';
import { Machine } from '../machine';

class Image extends DviCommand {
  img : string;
  
  constructor(img) {
    super({});
    this.img = img;
  }

  execute(machine : Machine) {
    machine.putImg( this.img );
  }
}

async function* specialsToImg(commands) {
  for await (const command of commands) {
    if (! command.special) {
      yield command;
    } else {
      if (! command.x.startsWith('pdf:image bbox ')) {
	yield command;
      } else {
	let svg = command.x.replace(/^dvisvgm:raw /, '');
	yield new SVG(svg);
      }
    }
  }
}
    
export default function (commands) {
  return merge( specialsToSVG(commands),
		command => command.svg,
		function*(commands) {
		  let svg = commands
		    .map( command => command.svg )
		    .join('')
                    .replace(/{\?nl}/g, "\n");
		  yield new SVG( svg );
		});
}
