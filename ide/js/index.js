

NProgress.configure({showSpinner: true});
let status = "init";
let localFS = undefined;
let PROJECT_PREFIX = "swiftlatex_";
let prefetchTemplateList = undefined;
let selectedTemplate = undefined;

function sanitizeString(str) {
    str = str.replace(/[^a-z0-9._-]/gim, "");
    return str.trim();
}

function validateEmail(email) {
    let re = /^(([^<>()\[\]\\.,;:\s@"]+(\.[^<>()\[\]\\.,;:\s@"]+)*)|(".+"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/;
    return re.test(String(email).toLowerCase());
}


function timestampToString(timestamp) {
    let newDate = new Date();
    newDate.setTime(timestamp);
    return newDate.toDateString();
}

function fetchToken() {
    return new Promise((resolve, reject) => {
        $.getScript("/oauth/token", function (data, textStatus, jqxhr) {
            if (jqxhr.status !== 200) {
                return reject("Not able to obtain token");
            }
            return resolve();
        }).fail(function () {
            return reject("Not able to obtain token");
        });
    });
}


async function deleteProject(pid) {
    status = "busy";
    NProgress.start();
    try {
        await localFS.delete("/" + pid);
        let data = await listProject();
        popupProjectList(data);
        status = "idle";
        NProgress.done();
    } catch (err) {
        fatal_error("Unable to remove project, please refresh to try again " + err);
    }

}

async function renameProject(pid, userProvidedName) {
    status = "busy";
    NProgress.start();
    let tbName = PROJECT_PREFIX + userProvidedName;
    try {
        await localFS.rename("/" + pid, "/" + tbName);
        let data = await listProject();
        popupProjectList(data);
        status = "idle";
        NProgress.done();
    } catch (err) {
        fatal_error("Unable to rename project, please refresh to try again " + err);
    }


}

function popupProjectList(projectlists) {
    $('#projectlistbody').html("");
    projectlists.forEach(function (element) {
        let projectHtml = `<tr data-pid='${element.project_id}'>
                                        <td class="projname">${element.project_name}</td>
                                        <td class="owner">${element.owner}</td>
                                        <td class="timestamp">${element.modified_time}</td>
                                        <td>
                                            <a href="#" data-pid='${element.project_id}' data-shareid='${element.share_id}' data-isshare='${element.share}' class='btn btn-info btn-sm openprojbtn'"> Edit</a>
                                            <a href="#" data-pid='${element.project_id}' class="btn btn-danger btn-sm removebtn"> Delete</a>
                                            <a href="#" data-pid='${element.project_id}' class="btn btn-success btn-sm renamebtn"> Rename</a>
                                            <a href="#" data-pid='${element.project_id}' class="btn btn-secondary btn-sm sharebtn"> Share</a>
                                        </td>
                                        </tr>`.trim();
        $('#projectlistbody').append(projectHtml);
    });

    $(".removebtn").click(function () {
        if (status !== 'idle') return;
        let pid = $(this).data("pid");
        if (confirm("Would you like to delete this project")) {
            deleteProject(pid);
        }
    });

    $(".renamebtn").click(function () {
        if (status !== 'idle') return;
        let pid = $(this).data("pid");
        let userProvideName = prompt("Rename your project", "New name for your project");
        if (userProvideName === null || userProvideName === undefined || userProvideName === "")
            return;
        userProvideName = sanitizeString(userProvideName);
        renameProject(pid, userProvideName);
    });

    $(".sharebtn").click(function () {

        if (status !== 'idle') return;
        if (window.oauth_type !== "google") {
            alert("Sorry. Currently only google supports the sharing feature.");
            return;
        }
        if (window.permission !== "full") {
            $('#permission-modal').modal('show');
            return;
        }


        let mail = prompt("Share this project with a friend via email?", "");
        if (mail === null || mail === undefined || mail === "")
            return;
        if (!validateEmail(mail)) {
            alert("It is an invalid email address");
            return;
        }

        let pid = $(this).data("pid");
        share_project(pid, mail);
    });

    $(".openprojbtn").click(function () {
        let shareid = $(this).data("shareid");
        let pid = $(this).data("pid");
        let isshare = $(this).data('isshare');

        if (window.oauth_type === "google") {
            if(isshare && window.permission !== "full") {
                $('#permission-modal').modal('show');
                return;
            }
        }
        window.location.href = `ide.html?pid=${pid}&share_id=${shareid}&share=${isshare}`
    });

}

async function share_project(pid, mail) {

    status = "busy";
    NProgress.start();
    try {
        await localFS.share(pid, mail);
        alert("The project is now shared with " + mail);
    } catch (err) {
        alert("Unable to share the project due to " + err);
    }

    status = "idle";
    NProgress.done();
}

function fatal_error(reason) {
    status = "error";
    $('#alert-modal-body').html("<p>" + reason + "</p>");
    $('#alert-modal').modal('show');
}


$("#createprojbtn").click(function () {
    if (status !== "idle") return;
    fetchTemplateList();
    $('#createProjectModal').modal({
        keyboard: true
    });
});

function fetchTemplateList() {
    if(prefetchTemplateList === undefined) {
        prefetchTemplateList = new Bloodhound({
          datumTokenizer: Bloodhound.tokenizers.obj.whitespace('name'),
          queryTokenizer: Bloodhound.tokenizers.whitespace,
          prefetch: './templates/list.json'
        });
        $('#templatelist').typeahead(null, {
          name: 'best-pictures',
          display: 'name',
          source: prefetchTemplateList,
          templates: {
            empty: [
              '<div class="empty-message">',
                'unable to find any templates that match the current query, leave blank for default template',
              '</div>'
            ].join('\n'),
              suggestion: Handlebars.compile('<div><strong>{{name}}</strong> - {{cat}}<img src="templates/{{url}}.png" width="144px"/></div>')
          }
        });

        $('#templatelist').on('typeahead:selected', function(evt, item) {
            // do what you want with the item here
            selectedTemplate = item.url;
            //console.log(selectedTemplate);
        });
    }

}

async function listProject() {

    let entries = await localFS.readdir("/", false);
    let res = [];
    for (let i = 0; i < entries.length; i++) {
        let entry = entries[i];
        //console.log(entry);
        if (entry.isfolder && entry.name.startsWith(PROJECT_PREFIX)) {
            let owner = window.alternative_username;
            if (entry.uid !== undefined && entry.uid !== "")
                owner = entry.uid;
            let mtime = entry.mtime;
            let mtimeStr = timestampToString(mtime);
            let tmp = {
                'project_name': entry.name.substr(PROJECT_PREFIX.length), 'project_id': entry.name,
                'modified_time': mtimeStr, 'owner': owner, 'mtime': mtime, 'share':entry.share, 'share_id':entry.fid
            };
            res.push(tmp);
        }
    }
    res.sort(function (a, b) {
        return b.mtime - a.mtime;
    });
    return res;

}


async function requestCreateProject() //Too lazy to rewrite it as a promise
{
    let template = "templates/default.tex";
    if (status !== "idle") {
        return;
    }
    let userProvidedName = $("#projectnameinputfield").val();
    if (userProvidedName === undefined || userProvidedName === "") {
        userProvidedName = "Project" + Date.now();
    }

    userProvidedName = sanitizeString(userProvidedName);
    // let checkboxui = $(".templatecheckbox:checked");
    // if (checkboxui.length === 0) {
    //     //console.log("empty template");

    // } else {
    if(selectedTemplate) {
        template = "templates/" + selectedTemplate + ".zip";
    }
    
    let tbName = PROJECT_PREFIX + userProvidedName;
    let isthere = await localFS.exists("/" + tbName);
    if (isthere) {
        console.log("already exists");
        alert("This project already exists!");
        return;
    }
    status = "busy";
    NProgress.start();

    try {
        await localFS.mkdir("/" + tbName);
    } catch(error) {
        fatal_error('Looks like there was a problem when creating the project: ' + error);
        return;
    }

    try {
        if(template.endsWith(".tex"))
        {
            let templateText = `\\documentclass{article}\\begin{document}hello\\end{document}`;
            let response = await fetch(template);
            templateText = await response.text();
            await localFS.updateFile("/" + tbName + "/main.tex", templateText);
        } else {
            let response = await fetch(template);
            let fileHandle = await response.blob();
            let jszip = await JSZip.loadAsync(fileHandle);
            let tasks = [];
            jszip.forEach(function (relativePath, zipEntry) {
                console.log(relativePath);
                tasks.push([relativePath, zipEntry]);
            });
            for(let j = 0; j < tasks.length; j ++) {
                let relativePath = tasks[j][0];
                let zipEntry = tasks[j][1];
                if(zipEntry.isFolder) continue;
                if(relativePath.substr(1).indexOf("/") !== -1) continue;
                let binaryStr = await zipEntry.async("uint8array");
                await localFS.updateFile("/" + tbName + "/" + relativePath, binaryStr, true);
            }
        }
    } catch(err) {
        fatal_error('Unable to fetch template ' + err);
        return;
    }
    
    



    try {
        let data = await listProject();
        popupProjectList(data);
    } catch (error) {
        fatal_error('Looks like there was a problem when creating the project: ' + error);
        return;
    }

    status = "idle";
    NProgress.done();

}


function renewToken() {
    fetchToken().then(_ => {
        setTimeout(renewToken, 300000);
    }).catch(err => {
        console.error("Unable to refresh token" + err);
    });
}

function fetchVersion() {

    fetch('/version.txt')
        .then(response => {
            if (response.status === 200) {
                return response.text();
            } else {
                return "DEV";
            }
        })
        .then(data => {
            $("#versionTag").html(data.trim());
        })
        .catch(error => {
            console.error(error);
        })
}

function show_permission_banner() {
    if (window.permission === "full" || window.oauth_type !== "google") {
        $("#permission_alert").hide();
    }
}

async function init() {
    NProgress.start();
    try {
        await fetchToken();
        setTimeout(renewToken, 300000);
        $("#nametag").html(window.username);
    } catch (e) {
        fatal_error("Unable to fetch token " + e);
    }


    try {
        localFS = new SwiftFilesystem();
        await localFS.init();
    } catch (e) {
        fatal_error("Unable to connect to local storage " + e);
    }


    try {
        let data = await listProject();
        popupProjectList(data);
    } catch (e) {
        fatal_error("Unable to list project " + e);
    }

    show_permission_banner();

    status = "idle";
    NProgress.done();
}

fetchVersion();
init();




