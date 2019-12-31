

export interface Rule {
  a : number;
  b : number;
}

class Position {
  h: number;
  v: number;
  w: number;
  x: number;
  y: number;
  z: number;
  
  constructor(properties? : Position) {
    if (properties) {
      this.h = properties.h;
      this.v = properties.v;
      this.w = properties.w;
      this.x = properties.x;
      this.y = properties.y;
      this.z = properties.z;
    } else {
      this.h = this.v = this.w = this.x = this.y = this.z = 0;      
    }
  }
}

export class DviFont {
  name: string;
  checksum: number;
  scaleFactor: number;
  designSize: number;
  
  constructor(properties : DviFont) {
    this.name = properties.name;
    this.checksum = properties.checksum;
    this.scaleFactor = properties.scaleFactor;
    this.designSize = properties.designSize;
  }
}

export class Machine {
  fonts : DviFont[];
  font : DviFont;
  stack : Position[];
  position : Position;

  constructor () {
    this.fonts = [];
  }
  
  preamble ( numerator : number, denominator : number, magnification : number, comment : string ) {
  }

  pushColor( c : string ) {
  }

  popColor( ) {
  }  

  setPapersize( width : number, height : number ) {
  }
    
  push() {
    this.stack.push(new Position(this.position));
  }

  pop() {
    this.position = this.stack.pop();
  }

  beginPage( page : any ) {
    this.stack = [];
    this.position = new Position();
  }

  endPage() { }  

  post( p : any ) { }
  
  postPost( p : any ) { }
  
  putRule( rule : Rule ) {
  }

  moveRight( distance : number ) {
    this.position.h += distance;
  }

  moveDown( distance : number ) {
    this.position.v += distance;
  }

  setFont( font : DviFont ) {
    this.font = font;
  }

  putSVG( svg : string ) {
  }
  
  // Returns the width of the text
  putText( text : Buffer ) : number {
    return 0;
  }

 

  loadFont( properties : any ) : DviFont {
    var f = new DviFont(properties);
    
    return f;
  }
}

