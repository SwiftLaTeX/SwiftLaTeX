export default function dragEventIncludes({ dataTransfer }: DragEvent, item: string): boolean {
  if (!dataTransfer) {
    return false;
  }

  if (dataTransfer.types instanceof DOMStringList) {
    // dataTransfer.types is a DOMStringList on IE/Edge and not an Array as MDN will lead you to believe
    // DOMStringList doesn't have a `includes` method on those browsers
    // So we need to use `contains` to check for the presence of the string
    return dataTransfer.types.contains(item);
  }

  return dataTransfer.types.indexOf(item) > -1;
}
