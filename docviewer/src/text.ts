import { Machine, Rule } from "./machine";
import { Writable } from 'stream';

const epsilon = 0.00001;

export default class TextMachine extends Machine {
  output : Writable;
  snippets : [number, number, Buffer][];
  
  constructor( o : Writable ) {
    super();
    this.output = o;
    this.snippets = [];
  }

  putRule( rule : Rule ) {
  }
  
  beginPage( page : any ) {
    super.beginPage(page);
    this.snippets = [];
  }
  
  endPage() {
    this.snippets = this.snippets.sort( function(a,b) {
      if (a[1] < b[1]) return -1;
      if (a[1] > b[1]) return 1;

      if (a[0] < b[0]) return -1;
      if (a[0] > b[0]) return 1;

      return 0;
    });

    if (this.snippets.length == 0)
      return;
    
    let previousH = this.snippets[0][0];
    let previousV = this.snippets[0][1];
    
    for (let snippet of this.snippets) {
      let h = snippet[0];
      let v = snippet[1];
      let text = snippet[2];

      if (v > previousV)
	this.output.write("\n");

      if (h > previousH + epsilon)
	this.output.write(" ");      
      
      this.output.write(text.toString());

      previousV = v;
      previousH = h;
    }
  }  
  
  putText( text : Buffer ) : number {
    this.snippets.push( [ this.position.h, this.position.v, text ] );
    return epsilon;
  }

  postPost( p : any ) {
    this.output.end();    
  }
}
