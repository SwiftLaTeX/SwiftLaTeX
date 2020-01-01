

let from_editor = false;
let user_zoom_ratio = 1.5;


const get_erow = function (obj) {
    return parseInt(obj.attr("r"));
};

const get_ecol = function (obj) {
    return parseInt(obj.attr("c"));
};

const get_ecs = function (obj) {
    return parseInt(obj.attr("i"));
};

const get_fid = function (obj) {
    return parseInt(obj.attr("f"));
};

const setViewerHtml = function (data, pos, patch) {

    $("#viewer").html(data);

    $("data._rule").each(function () {
        //console.log($(this).attr("value"));
        let params = $(this).attr("value").split(" ");

        $(".pf").append(`<div style='z-index:-100; border-style: solid; border-width: 0.5px; position:absolute; left:${params[0]}px; bottom: ${params[1]}px; width: ${params[2]}px; height: ${params[3]}px;'></div>`)
    });

    $("data._color").each(function () {
        let color = $(this).attr("value");
        if (color !== '0 g 0 G') {
            let colorCount = 0;
            let ptr = $(this);
            while (colorCount < 1024) {
                colorCount += 1;
                ptr = ptr.next();
                if (ptr.length === 0) return;
                if (ptr.hasClass("_color")) return;
                ptr.css("color", "red");
            }

        }
    });

    $("._i").hover(function () {
        if (status !== "idle") return;
        let datasrc = $(this).attr("data-src");
        if (datasrc === "") return;
        $(this).attr("data-src", "");
        load_image(datasrc, this);
    }, function () {

    });

    handle_editor_move_anchor(pos, true);
    //console.log(patch);
    if (patch !== undefined) {
        if (patch.startsWith("+")) {
            let tmpk;
            for (tmpk = 1; tmpk < patch.length; tmpk++) {
                handle_editor_char_insert(patch[tmpk]);
            }
        } else if (patch.startsWith("-")) {
            let tmpk;
            for (tmpk = 1; tmpk < patch.length; tmpk++) {
                handle_editor_char_delete(patch[tmpk]);
            }
        }
    }


    install_mouse_event_handler();

};

const load_image = async function (path, imgobj) {
    if (status !== "idle") return;
    NProgress.start();
    status = "busy";
    let fid = path;
    console.log(path);
    if (fid !== undefined) {
        let tmpRes = await localFS.getFile(path, true);
        let tmpBlob = await generatePDFThumbnail(tmpRes);
        let objectURL = URL.createObjectURL(tmpBlob);
        //console.log(objectURL);
        imgobj.src = objectURL;
        imgobj.onload = function () {
            URL.revokeObjectURL(objectURL);
        };
        imgobj.onerror = function () {
            URL.revokeObjectURL(objectURL);
        }
    }
    NProgress.done();
    status = "idle";
};

const clear_cursor = function () {

    $(".viewercursor").remove();

};

const show_cursor = function (span_obj, before) {
    //console.log("clear cursor");
    clear_cursor();
    let isspace = span_obj.hasClass("sp");

    if (before) {
        span_obj.before(`<span class='viewercursor' r=${get_erow(span_obj)} c=${get_ecol(span_obj)} f=${get_fid(span_obj)}></span>`);
    } else {
        span_obj.after(`<span class='viewercursor' r=${get_erow(span_obj)} c=${get_ecol(span_obj)} f=${get_fid(span_obj)}></span>`);
    }

    if (!from_editor) {
        if (before) {
            if (!isspace)
                handleViewerAnchorMoved({
                    'row': get_erow(span_obj),
                    'column': get_ecol(span_obj) - 1,
                    'file': get_fid(span_obj),
                    'type': 'normal'
                });
            else
                handleViewerAnchorMoved({
                    'row': get_erow(span_obj),
                    'column': get_ecol(span_obj),
                    'file': get_fid(span_obj),
                    'type': 'spacebefore'
                });
            // if(!isspace)
            //     ide.editor.postMessage({"cmd":"viewermoveanchor", "data": {'row': get_erow(span_obj), 'column': get_ecol(span_obj) - 1, 'type':'normal'}}, "*");
            // else
            //     ide.editor.postMessage({"cmd":"viewermoveanchor", "data": {'row': get_erow(span_obj), 'column': get_ecol(span_obj), 'type':'spacebefore'}}, "*");
        } else {
            if (!isspace)
                handleViewerAnchorMoved({
                    'row': get_erow(span_obj),
                    'column': get_ecol(span_obj),
                    'file': get_fid(span_obj),
                    'type': 'normal'
                });
            else
                handleViewerAnchorMoved({
                    'row': get_erow(span_obj),
                    'column': get_ecol(span_obj),
                    'file': get_fid(span_obj),
                    'type': 'spaceafter'
                });
            // if(!isspace)
            //     ide.editor.postMessage({"cmd":"viewermoveanchor", "data": {'row': get_erow(span_obj), 'column': get_ecol(span_obj), 'type':'normal'}}, "*");
            // else
            //     ide.editor.postMessage({"cmd":"viewermoveanchor", "data": {'row': get_erow(span_obj), 'column': get_ecol(span_obj), 'type':'spaceafter'}}, "*");
        }

    }

};

const is_cursor_showing = function () {
    return ($(".viewercursor").length > 0);
};


const set_viewer_bg_color = function (color) {
    $("#viewer").css("background-color", color);
};

const handle_character_span_click = function (span_obj, event_obj) {
    if (status === "busy") return;
    from_editor = false;
    if (get_ecs(span_obj) === 0 && get_erow(span_obj) !== 0) {
        if (event_obj.pageX > span_obj.offset().left + span_obj.width() / 2) {
            show_cursor(span_obj, false);
        } else {
            show_cursor(span_obj, true);
        }
    } else if (get_ecs(span_obj) !== 0 && get_erow(span_obj) !== 0) {
        clear_cursor();
        handleViewerAnchorMoved({
            'row': get_erow(span_obj),
            'column': get_ecol(span_obj),
            'file': get_fid(span_obj),
            'type': 'command'
        });
        //ide.editor.postMessage({"cmd":"viewermoveanchor", "data": {'row': get_erow(span_obj), 'column': get_ecol(span_obj), 'type':'command'}}, "*");
    }
};

const install_mouse_event_handler = function () {

    $(".tl>span").click(function (event_obj) {

        handle_character_span_click($(this), event_obj);
    });


};


const handle_editor_move_anchor = function (pos, isrestore) {
    if (pos === undefined) return;
    from_editor = true;
    let row = pos.row + 1;
    let column = pos.column;
    let fid = pos.fid;
    if (fid === 0 || fid === undefined) {
        //console.warn("Maybe a bug!");
        return;
    }
    //console.log("Looking for fid " + fid + " row " + row + " column " + column);
    let candicates = $(`span[r='${row}']`);
    let is_before = false;
    let obj = candicates.filter(function () {
        return get_ecol($(this)) === column && get_ecs($(this)) === 0 && get_fid($(this)) === fid;
    });

    if (obj.length > 0) {
        //console.log("show cursor from this");
        show_cursor(obj.eq(0), is_before);
        return;
    }


    //console.log("Looking for " + row + " " + column);
    //Find the nearest one in the same
    obj = undefined;
    let min_distance = 0xffff;
    candicates.each(function (index, element) {
        if (get_ecs($(this)) !== 0) return;
        if (get_fid($(this)) !== fid) return;
        let distance = get_ecol($(this)) - column;
        if (Math.abs(distance) <= min_distance) {
            min_distance = Math.abs(distance);
            obj = $(this);
            is_before = distance > 0;
        }
    });
    if (obj !== undefined && obj.length > 0) {
        // console.log(obj);

        show_cursor(obj, is_before);
        if (isrestore) {
            obj.after("<span>&nbsp</span>");
        }

    } else {
        //console.log("not such a cursor");
    }

};

const handle_editor_char_insert = function (code) {
    from_editor = true;
    if (!is_cursor_showing()) {
        //console.error("reject this char " + code);
        return;
    }
    let cursorobj = $(".viewercursor");
    let orirow = get_erow(cursorobj);
    let oricol = get_ecol(cursorobj);
    let orifid = get_fid(cursorobj);
    if (code === " ") code = "&nbsp";
    cursorobj.before(`<span i=0 r=${orirow} c=${oricol} f=${orifid}>${code}</span>`);
    cursorobj.attr('c', oricol + 1);

    let nextobj = cursorobj;
    let stepCount = 0;
    while (stepCount < 255) {
        stepCount += 1;
        nextobj = nextobj.next();
        if (nextobj.length === 0)
            break;
        // if(!nextobj.hasClass("_it"))
        //     break;

        let tmpcol = get_ecol(nextobj) + 1;
        nextobj.attr('c', tmpcol);
    }
};

const handle_editor_char_delete = function (code) {
    from_editor = true;
    if (!is_cursor_showing()) {
        //console.error("reject this char " + code);
        return;
    }

    let cursorobj = $(".viewercursor");
    let orirow = get_erow(cursorobj);
    let oricol = get_ecol(cursorobj);
    if (oricol === 1)
        return;

    let prevspan = cursorobj.prev();
    if (prevspan.length === 0)
        return;

    //console.log(code + "   " + prevspan.html() + " " + prevspan.hasClass("sp"));
    if (code === " " && prevspan.hasClass("sp")) {
        prevspan.remove();
        //console.log("remove ?");
    } else if (prevspan.html() === code) {
        prevspan.remove();
    }
};


const zoomin_handler = function () {
    let pagec = $(".pf");
    user_zoom_ratio = user_zoom_ratio + 0.1;
    pagec.css("-webkit-transform", `scale(${user_zoom_ratio})`);
    pagec.css("-ms-transform", `scale(${user_zoom_ratio})`);
    pagec.css("-transform", `scale(${user_zoom_ratio})`);
};

const zoomout_handler = function () {
    let pagec = $(".pf");
    user_zoom_ratio = user_zoom_ratio - 0.1;
    pagec.css("-webkit-transform", `scale(${user_zoom_ratio})`);
    pagec.css("-ms-transform", `scale(${user_zoom_ratio})`);
    pagec.css("-transform", `scale(${user_zoom_ratio})`);
};


const generatePDFThumbnail = async function (uint8array) {

    try {
        let doc = await pdfjsLib.getDocument(uint8array).promise;
        let page = await doc.getPage(1);
        let viewport = page.getViewport({scale: 1});
        let canvas = document.getElementById('pdfcanvas');
        let context = canvas.getContext('2d');
        canvas.height = viewport.height;
        canvas.width = viewport.width;
        let renderContext = {
            canvasContext: context,
            viewport: viewport
        };
        await page.render(renderContext).promise;
        let newblob = await new Promise((resolve, reject) => {
            canvas.toBlob(function (jb) {
                return resolve(jb); // this line should be here
            }, 'image/png');
        });
        doc.destroy();
        return newblob;
    } catch (e) {
        let defaultPixel = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR42mNk+M9QDwADhgGAWjR9awAAAABJRU5ErkJggg==";
        let res = await fetch(defaultPixel);
        let defaultBlob = await res.blob();
        console.error("Unable to open pdf document, return a dummy pdf blob " + e);
        return defaultBlob;
    }

};


//ide.postMessage({'cmd':'viewerloaded'}, '*');


