const TEXCACHEROOT = "/tex";
const WORKROOT = "/work";
var Module = {};
let memlog = "";
let mainFileName = "main.tex";
Module['print'] = function(a) { memlog += a + "\n"; };
// Module['printErr'] = function(a) { console.log(a); }
Module['preRun'] = function(){
          FS.mkdir(TEXCACHEROOT);
          FS.mkdir(WORKROOT);
};

Module['postRun'] = function(){
    //Dump the xelatex.fmt
    // var myArray = FS.readFile("xelatex.fmt");
    // var xhr = new XMLHttpRequest;
    // xhr.open("POST", "/dump", false);
    // xhr.send(myArray);
    self.postMessage({'result':'ok', 'cmd':"postRun"});
};

function cleanDir(dir)
{
      let l = FS.readdir(dir);
      for(let i in l) {
          let item  = l[i];
          if(item === "." || item === "..") { continue; }
          item = dir + "/" + item;
          let fsStat = undefined;
          try {
              fsStat = FS.stat(item);
          } catch(err){
              console.error("Not able to fsstat " + item);
              continue;
          }
          if(FS.isDir(fsStat.mode))
          {
                    cleanDir(item);
          }
          else
          {
            try
            {
              FS.unlink(item);
            }
            catch(err){
                        console.error("Not able to unlink " + item);
            }
          }
      }

      if(dir !== WORKROOT) {
        try {
          FS.rmdir(dir);
        } catch(err){
          console.error("Not able to top level " + dir);
        }
      }
}

function exitEngine(status, isabort)
{
  let logArrayBuffer = null;
  let pdfArrayBuffer = null;

  if(isabort)
  {
      console.error("Engine crashed\n");
      self.postMessage({'result':'failed', 'cmd':"compile", 'info':'Engine crashed.', 'status':status, 'log': memlog});
      close();
      return;
  }

  if(status === 0)
  {
      try
      {
          let pdfurl = WORKROOT+ "/_input_.xdv";
          pdfArrayBuffer = FS.readFile(pdfurl, { encoding: 'binary' }); //We assume it will generate something
      }
      catch(err)
      {
          console.error("Unable to get the output file, fallback to error routine");
          self.postMessage({'result':'ok', 'cmd':"compile", 'info':'Compilation finished, but no outputfile',
           'status':1,  'log': memlog});
          return;
      }
      self.postMessage({'result':'ok', 'cmd':"compile", 'info':'Compilation finished', 'status':status, 'log':memlog,
                'pdf': pdfArrayBuffer.buffer}, [pdfArrayBuffer.buffer]);
  }
  else
  {
      console.error("Compilation failed, clearing cache\n");
      self.postMessage({'result':'ok', 'cmd':"compile", 'info':'compilation failed', 'status':status, 'log':memlog});
  }
}



Module['onAbort'] = function(){
  exitEngine(-1, true); //Restart the engine. Oops
};


function _ensure_bootstrap_link_exist()
{
    try { //Unlink first 
          FS.unlink(WORKROOT+ "/_input_.tex");
    } catch(err) {
          //Dont bother
    }
    FS.symlink(WORKROOT+ "/" + self.mainFileName, WORKROOT+ "/_input_.tex"); //It may fail?
}


self['onmessage'] = function(ev) {
  let data = ev['data'];
  let cmd = data['cmd'];
  if(cmd === 'compile')
  {
        memlog = ""; //Clean the log
        _ensure_bootstrap_link_exist();
        FS.chdir(WORKROOT);
        let r = _compileLaTeX();
        if(r == 0)
        {
            _compileBib();
        }
        exitEngine(r, false); 
  }

  else if(cmd === "mkdir") {
      try
      {
          //console.log("removing " + item);
          FS.mkdir(WORKROOT + "/" + data['url']);
          self.postMessage({'result':'ok', 'cmd':cmd, 'info': data['url']});
      }
      catch(err){
        console.error("Not able to mkdir " + data['url']);
        self.postMessage({'result':'failed', 'cmd':cmd, 'info': data['url']});
      }
  }
  else if(cmd === "writefile")
  {
      try {
        FS.writeFile(WORKROOT+ "/" + data['url'], data['src']);
        self.postMessage({'result':'ok', 'cmd':cmd, 'info': data['url']});
      } catch(err) {
        console.error("Unable to write mem file");
        self.postMessage({'result':'failed', 'cmd':cmd, 'info': 'Write file failed'});
      }
  }
  else if(cmd === "setmainfile")
  {
      self.mainFileName = data['url'];
      self.postMessage({'result':'ok', 'cmd':cmd, 'info': data['url']});
  }
  else if(cmd === "grace")
  {
      console.error("Gracefully Close");
      self.close();
      self.postMessage({'result':'ok', 'cmd':cmd, 'info':'Engine is now closed'});
  }
  else if(cmd === "flushcache")
  {
      cleanDir(WORKROOT);
      self.postMessage({'result':'ok', 'cmd':cmd, 'info': 'MemFS is flushed'});
  }
  else
  {
      self.postMessage({'result':'failed', 'cmd':cmd, 'info':'Unknown command'});
  }
};



let texlive404_cache = {};
const formatFilters = [".log", ".aux", ".toc", ".bbl", ".jpg", ".pdf", ".bmp", ".bb", ".png"]
function kpse_fetch_from_network_impl(nameptr)
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
  const remote_endpoint = "https://www.swiftlatex.com/dl/tex/";
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


