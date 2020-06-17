/**
 * Get relative path of target file from source file
 */
export default function getRelativePath(source: string, target: string, separator: string = '/') {
  const targetArr = target.split(separator);
  const sourceArr = source.split(separator).slice(0, -1);
  const filename = targetArr.pop();
  const targetPath = targetArr.join(separator);

  let relativePath = '';

  while (targetPath.indexOf(sourceArr.join(separator)) === -1) {
    sourceArr.pop();
    relativePath += '..' + separator;
  }

  const relPathArr = targetArr.slice(sourceArr.length);

  if (relPathArr.length) {
    relativePath += relPathArr.join(separator) + separator;
  }

  if (!relativePath.startsWith('.')) {
    relativePath = './' + relativePath;
  }

  return relativePath + filename;
}
