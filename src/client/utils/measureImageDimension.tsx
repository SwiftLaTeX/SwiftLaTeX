import { getDocument, GlobalWorkerOptions } from 'pdfjs-dist';

export async function measureImageDimension(content: Blob, name: string): Promise<string> {
  if (name.endsWith('pdf')) {
    GlobalWorkerOptions.workerSrc =
      '//cdn.jsdelivr.net/npm/pdfjs-dist@2.4.456/build/pdf.worker.min.js';
    const tempPDFURL = URL.createObjectURL(content);
    try {
      const pdfDoc = await getDocument(tempPDFURL).promise;
      const page = await pdfDoc.getPage(1);
      const viewport = page.getViewport({ scale: 1 });
      const tmpWidth = viewport.width;
      const tmpHeight = viewport.height;
      pdfDoc.destroy();
      return `swiftlatex\n${tmpWidth}\n${tmpHeight}\n`;
    } catch (e) {
      console.error(`Unable to measure the pdf document ${name}, return a dummy dimension ` + e);
      return 'swiftlatex\n300\n400\n';
    } finally {
      URL.revokeObjectURL(tempPDFURL);
    }
  } else {
    return new Promise((resolve, _) => {
      const tempImageURL = URL.createObjectURL(content);
      const tmpImgElement = new Image();
      tmpImgElement.onload = _ => {
        URL.revokeObjectURL(tempImageURL);
        resolve(`swiftlatex\n${tmpImgElement.width}\n${tmpImgElement.height}\n`);
      };
      tmpImgElement.onerror = _ => {
        console.error(`Unable to measure the image document ${name}, return a dummy dimension `);
        URL.revokeObjectURL(tempImageURL);
        resolve('swiftlatex\n300\n400\n');
      };
      tmpImgElement.src = tempImageURL;
    });
  }
}
