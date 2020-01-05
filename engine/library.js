mergeInto(LibraryManager.library, {
  kpse_fetch_from_network: function(nameptr) {
    return kpse_fetch_from_network_impl(nameptr);
  }
});

let texlive404_cache = {};
const formatFilters = [".log", ".aux", ".toc", ".bbl", ".jpg", ".pdf", ".bmp", ".bb", ".png"]
function kpse_fetch_from_network(nameptr)
{

  const reqname = UTF8ToString(nameptr);
  if(reqname in texlive404_cache) return 0;
  if(reqname.includes("/") || reqname.includes(" ")) return 0;
  for(let i = 0; i < formatFilters.length; i ++) {
      if(reqname.endsWith(formatFilters[i])) {
        return 0;
      }
  }

  //self.postMessage({'result':'ok', 'type':'status', 'cmd':'texlivefetch', 'data':reqname});
  const remote_endpoint = "http://127.0.0.1:5000/tex/";
  let xhr = new XMLHttpRequest();
  xhr.open("GET", remote_endpoint + reqname, false);
  xhr.timeout = 15000;
  xhr.responseType = "arraybuffer";
  console.log("Start downloading texlive file " + TEXCACHEROOT + "/" + reqname);
  xhr.send();
  if (xhr.status === 200) {

    let arraybuffer = xhr.response;
    //console.log(arraybuffer);
    FS.writeFile(TEXCACHEROOT + "/" + reqname, new Uint8Array(arraybuffer));
    console.log("Downloaded texlive file " + TEXCACHEROOT + "/" + reqname);
    return 1;
  }
  else if(xhr.status === 301 || xhr.status === 404)
  {
    console.log("TexLive File not exists " + reqname);
    texlive404_cache[reqname] = 1;
    return 0;
  }
  return -1;
}