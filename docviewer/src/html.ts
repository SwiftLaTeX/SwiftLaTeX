import { Machine, Rule } from "./machine";
import { Writable } from 'stream';

export default class HTMLMachine extends Machine {
  output : Writable;
  pointsPerDviUnit : number;

  svgDepth : number;
  color : string;
  colorStack : string[];

  paperwidth : number;
  paperheight : number;
    
  pushColor( c : string ) {
    this.colorStack.push(this.color);
    this.color = c;
  }

  popColor( ) {
    this.color = this.colorStack.pop();
  }

  setPapersize( width : number, height : number ) {
    this.paperwidth = width;
    this.paperheight = height;  
  }

  putSVG( svg : string ) {
    let left = this.position.h * this.pointsPerDviUnit;
    let top = this.position.v * this.pointsPerDviUnit;

    this.svgDepth += (svg.match(/<svg>/g) || []).length;
    this.svgDepth -= (svg.match(/<\/svg>/g) || []).length;
    
    svg = svg.replace("<svg>", `<svg width="10pt" height="10pt" viewBox="-5 -5 10 10" style="overflow: visible; position: absolute;">`);
    
    svg = svg.replace(/{\?x}/g, left.toString());
    svg = svg.replace(/{\?y}/g, top.toString());
    
    this.output.write( svg );
  }
  
  constructor( o : Writable ) {
    super();
    this.output = o;
    this.color = 'black';
    this.colorStack = [];
    this.svgDepth = 0;
  }

  preamble ( numerator : number, denominator : number, magnification : number, comment : string ) {
    let dviUnit = magnification * numerator / 1000.0 / denominator;
    
    let resolution = 300.0; // ppi
    let tfm_conv = (25400000.0 / numerator) * (denominator / 473628672) / 16.0;
    let conv = (numerator / 254000.0) * (resolution / denominator);
    conv = conv * (magnification / 1000.0);
    
    this.pointsPerDviUnit = dviUnit * 72.27 / 100000.0 / 2.54;
  }
  
  putRule( rule : Rule ) {
    let a = rule.a * this.pointsPerDviUnit;
    let b = rule.b * this.pointsPerDviUnit;
    let left = this.position.h * this.pointsPerDviUnit;
    let bottom = this.position.v * this.pointsPerDviUnit;
    let top = bottom - a;
    
    this.output.write(`<span style="background: ${this.color}; position: absolute; top: ${top}pt; left: ${left}pt; width:${b}pt; height: ${a}pt;"></span>\n`);
  }
    
  putText( text : Buffer ) : number {
    return 0;
  }

  
}

