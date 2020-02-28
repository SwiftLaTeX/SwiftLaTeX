NProgress.configure({showSpinner: true});
let prefetch_template_list = undefined;
let selected_template = undefined;
const ALLOWED_MAX_FILESIZE = 2048 * 1024;

function fetchTemplateList() {
    if (prefetch_template_list === undefined) {
        prefetch_template_list = new Bloodhound({
            datumTokenizer: Bloodhound.tokenizers.obj.whitespace('name'),
            queryTokenizer: Bloodhound.tokenizers.whitespace,
            prefetch: '/static/templates/list.json'
        });
        let template_list_control = $('#template_list');
        template_list_control.typeahead(null, {
            name: 'best-pictures',
            display: 'name',
            source: prefetch_template_list,
            templates: {
                empty: [
                    '<div class="empty-message">',
                    'unable to find any templates that match the current query, leave blank for default template',
                    '</div>'
                ].join('\n'),
                suggestion: Handlebars.compile('<div><strong>{{name}}</strong> – {{cat}}<img src="/static/templates/{{url}}.png" width="144px"/></div>')
            }
        });

        template_list_control.on('typeahead:selected', function (evt, item) {
            selected_template = item.url;
        });
    }
}

async function _uploadZip(fileHandle, s3fs) {
    let jszip = await JSZip.loadAsync(fileHandle);
    const allowed_extension = ['.tex', '.bib', '.bst', '.cls', '.pdf', '.jpg', '.png', '.txt', '.bbx', 'sty'];
    let queues = [];
    jszip.forEach(function (relativePath, zipEntry) {
        // console.log(zipEntry);
        if (zipEntry.size > ALLOWED_MAX_FILESIZE) {
            console.warn(`File ${relativePath} is too large, ignored`);
            return;
        }
        if (zipEntry.dir) {

        } else {
            let legalExtension = false;
            allowed_extension.some(ext => {
                if (zipEntry.name.endsWith(ext)) {
                    legalExtension = true;
                    return true;
                }
            });
            if (!legalExtension) {
                console.warn(`File ${relativePath} has illegal extension, ignored`);
                return;
            }
            queues.push(['/' + relativePath, zipEntry]);
        }
    });
    for (let j = 0; j < queues.length; j++) {
        // console.log("Uploading the file " + queues[j][0]);
        $("#status").prepend(`<p>Uploading the file: ${queues[j][0]}</p>`);
        try {
            // It is dir

            const dstpath = queues[j][0];
            const zipentry = queues[j][1];
            // console.log("ensuring " + s3fs._getParentDir(dstpath));
            await s3fs.ensureDirExist(window.polyfillDirPath(dstpath));
            // console.log("writing " + dstpath);
            let binaryStr = await zipentry.async("uint8array");
            await s3fs.writeFile(dstpath, binaryStr);

        } catch (ex) {
            console.error("Error detected when uploading the file " + queues[j][0] + " " + ex);
            $("#status").prepend(`<p>Error detected when uploading the file: ${queues[j][0]}</p>`);
        }
    }
}

async function uploadBuiltinTemplate() {
    let s3fs = window.s3fs;
    if (s3fs) {
        try {
            let template_url = '/static/templates/' + selected_template + '.zip';
            let response = await fetch(template_url);
            let fileHandle = await response.blob();
            await _uploadZip(fileHandle, s3fs);
        } catch (e) {
            console.error("Error detected when uploading templates " + e);
        }
    }
    $("#status").prepend('<p>Done! </p>');
    setTimeout(function () {
        window.location.href = "/project";
    }, 3000);
}

async function uploadUserZip(fileHandle) {
    let s3fs = window.s3fs;
    if (s3fs) {
        await _uploadZip(fileHandle, s3fs);
    }
    $("#status").prepend('<p>Done! </p>');
    setTimeout(function () {
        window.location.href = "/project";
    }, 3000);
}


fetchTemplateList();

$("#upload_template_button").click(_ => {
    if (selected_template) {
        uploadBuiltinTemplate();
        NProgress.start();
    } else {
        let fileinput = document.getElementById('fileToUpload');
        if (fileinput.files.length > 0) {
            let fileHandle = fileinput.files[0];
            uploadUserZip(fileHandle);
            NProgress.start();
        }
    }
});

$("#upload_default_button").click(_ => {
    selected_template = "default";
    uploadBuiltinTemplate();
    NProgress.start();
});
