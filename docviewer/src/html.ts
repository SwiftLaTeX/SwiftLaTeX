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
    
  _to_legal_unicode (c : number): number {
    if ((c <= 0x20) || (c >= 0x7F && c <= 0xA0) || (c == 0xAD)) {
      return c + 0xE000;
    } else {
      return c;
    }
  }

  putText( text : Buffer ) : number {
    
        let textWidth = 0;
        let textHeight = 0;
        let textDepth = 0;

        var htmlText = "";
        
        
        for( let i = 0; i < text.length; i++ ) {
          let c = text[i];
          //console.log(c);
          let metrics = this.font.metrics.characters[c];
          if (metrics === undefined)
            throw Error(`Could not find font metric for ${c}`);
                
          textWidth += metrics.width;
          textHeight = Math.max(textHeight, metrics.height);
          textDepth = Math.max(textDepth, metrics.depth);

          c = this._to_legal_unicode(c);
          
          htmlText += String.fromCharCode(c);
         
          //console.log(c);
        }
        
        // tfm is based on 1/2^16 pt units, rather than dviunit which is 10^−7 meters
        var dviUnitsPerFontUnit = this.font.metrics.designSize / 1048576.0 * 65536 / 1048576;
        
        var top = (this.position.v - textHeight * dviUnitsPerFontUnit) * this.pointsPerDviUnit;
        let left = this.position.h * this.pointsPerDviUnit;

        var width = textWidth * this.pointsPerDviUnit * dviUnitsPerFontUnit;
        var height = textHeight * this.pointsPerDviUnit * dviUnitsPerFontUnit;
        var depth = textDepth * this.pointsPerDviUnit * dviUnitsPerFontUnit;
        var top = this.position.v * this.pointsPerDviUnit;

        let fontsize = (this.font.metrics.designSize / 1048576.0) * this.font.scaleFactor / this.font.designSize;

        if (this.svgDepth == 0) {
      this.output.write( `<span style="line-height: 0; color: ${this.color}; font-family: ${this.font.name}; font-size: ${fontsize}pt; position: absolute; top: ${top - height}pt; left: ${left}pt; overflow: visible;"><span style="margin-top: -${fontsize}pt; line-height: ${0}pt; height: ${fontsize}pt; display: inline-block; vertical-align: baseline; ">${htmlText}</span><span style="display: inline-block; vertical-align: ${height}pt; height: ${0}pt; line-height: 0;"></span></span>\n` );
        } else {
          let bottom = this.position.v * this.pointsPerDviUnit;
          // No 'pt' on fontsize since those units are potentially scaled
          this.output.write( `<text alignment-baseline="baseline" y="${bottom}" x="${left}" style="font-family: ${this.font.name};" font-size="${fontsize}">${htmlText}</text>\n` );
        }
        
        return textWidth * dviUnitsPerFontUnit * this.font.scaleFactor / this.font.designSize;
  }


  putNativeText( text : Buffer ) : number {
    return 0;
  }  
}

