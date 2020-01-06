import { DviCommand } from '../parser';
import { Machine } from '../machine';

class PushColor extends DviCommand {
  color : string;
  
  constructor(color) {
    super({});
    this.color = color;
  }

  execute(machine : Machine) {
    machine.pushColor( this.color );
  }
  
  toString() : string {
    return `PushColor { color: '${this.color}' }`;
  }  
}

class PopColor extends DviCommand {
  constructor() {
    super({});
  }  

  execute(machine : Machine) {
    machine.popColor();
  }

  toString() : string {
    return `PopColor { }`;
  }  
}

function intToHex(n) {
  return ("00" + Math.round(n).toString(16)).substr(-2);
}

function texColor(name) {
  if (name == 'gray 0')
    return 'black';
  if (name == 'gray 1')
    return 'white';
  if (name.startsWith('rgb ')) {
    return '#' + name.split(' ').slice(1).map(function (x) { return intToHex(parseFloat(x) * 255); }).join('');
  }
  if (name.startsWith('gray ')) {
    var x = name.split(' ')[1];
    return texColor('rgb ' + x + ' ' + x + ' ' + x);
  }
  return 'black';
}

export default async function* (commands) {
  let queue = [];

  for await (const command of commands) {
    if (! command.special) {
      yield command;
    } else {
      if (! command.x.startsWith('color ')) {
	yield command;
      } else {
        if (command.x.startsWith('color push ')) {
	  let color = texColor(command.x.replace(/^color push /, ''));
	  yield new PushColor(color);
	}

        if (command.x.startsWith('color pop')) {
	  yield new PopColor();
        }
      }
    }
  }
}

