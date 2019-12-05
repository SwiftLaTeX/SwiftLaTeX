 NProgress.configure({ showSpinner: true });
 let status = "init";
 let networkClient = undefined;
 function sanitizeString(str){
                str = str.replace(/[^a-z0-9._-]/gim,"");
                return str.trim();
 }

 function validateEmail(email) {
            let re = /^(([^<>()\[\]\\.,;:\s@"]+(\.[^<>()\[\]\\.,;:\s@"]+)*)|(".+"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/;
            return re.test(String(email).toLowerCase());
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

function buildNetworkClient()
{
    if(oauth_type === "dropbox")
    {
        return buildDropboxClient();
    }
    else if(oauth_type === "google")
    {
        return buildGoogleClient();
    }
    else {
        throw "not implemented client";
        //return window.postMessage({'cmd':'networkclientfail'}, '*');
    }
}



function popupProjectList(projectlists)
{
            $('#projectlistbody').html("");
            projectlists.forEach(function(element){
                        let projectHtml = `<tr data-pid='${element.project_id}'>
                                        <td class="projname">${element.project_name}</td>
                                        <td class="owner">${element.owner}</td>
                                        <td class="timestamp">${element.modified_time}</td>
                                        <td>
                                            <a href="ide.html?pid=${element.project_id}" class='btn btn-info btn-sm'"> Edit</a>
                                            <a href="#" data-pid='${element.project_id}' class="btn btn-danger btn-sm removebtn"> Delete</a>
                                            <a href="#" data-pid='${element.project_id}' class="btn btn-success btn-sm renamebtn"> Rename</a>
                                            <a href="${element.project_url}" target='_blank' class="btn btn-info btn-sm"> Manage</a>
                                                <a href="#" data-pid='${element.project_id}' class="btn btn-secondary btn-sm sharebtn"> Share</a>
                                        </td>
                                        </tr>`.trim();
                        $('#projectlistbody').append(projectHtml);
            });

            $(".removebtn").click(function(){
                if (status !== 'idle') return;
                let pid = $(this).data("pid");
                if(confirm("Would you like to delete this project"))
                {
                    status = "busy";
                    NProgress.start();
                    networkClient.deleteProject(pid).then(res => {
                        return networkClient.listProject();
                    }).then(data => {
                        popupProjectList(data);
                        status = "idle";
                        NProgress.done();
                    }).catch(err => {
                        fatal_error("Unable to remove project, please refresh to try again " + err);
                        
                    });
                }
             });

            $(".renamebtn").click(function(){
                if (status !== 'idle') return;
                let pid = $(this).data("pid");
                let newpid = prompt("Rename your project", "New name for your project");
                if(newpid === null || newpid === undefined || newpid === "")
                    return;
                newpid = sanitizeString(newpid);
                status = "busy";
                NProgress.start();
                networkClient.renameProject(pid, newpid).then(res => {
                        return networkClient.listProject();
                    }).then(data => {
                        popupProjectList(data);
                        status = "idle";
                        NProgress.done();
                    }).catch(err => {
                        fatal_error("Unable to remove project, please refresh to try again " + err);
                    });

            });

            $(".sharebtn").click(function () {
                if (status !== 'idle') return;
                if(window.oauth_type === "dropbox")
                {
                    alert("Sorry, Dropbox does not support the sharing feature.");
                    return;
                }
                let pid = $(this).data("pid");
                let mail = prompt("Share this project with a friend via email?", "");
                if (mail === null || mail === undefined || mail === "")
                    return;
                if(!validateEmail(mail))
                    return;
                status = "busy";
                NProgress.start();
                networkClient.shareProject(pid, mail).then(res => {
                        alert("All done~, this project has been shared to " + mail);
                        status = "idle";
                        NProgress.done();
                    }).catch(err => {
                        alert("Unable to share the project due to " + err);
                        status = "idle";
                        NProgress.done();
                        //fatal_error("Unable to share the project due to" + err);
                    });
                // window.postMessage({'cmd':'shareprojreq', 'data':[pid, mail]}, '*');

            });

}

function fatal_error(reason)
{
    status = "error";
              $('#alert-modal-body').html("<p>" + reason + "</p>");
              $('#alert-modal').modal('show');
}





$("#createprojbtn").click(function(){
        if(status !== "idle") return;
            fetchTemplateList();
            $('#createProjectModal').modal( {
                  keyboard: true
            });
});

function fetchTemplateList()
{
            fetch('templates/list.json')
                .then(function(response) {
                  if (!response.ok) {
                    throw Error(response.statusText);
                  }
                  // Read the response as json.
                  return response.json();
                })
                .then(function(responseAsJson) {
                    let listdata = responseAsJson['data'];
                    let templateUI = $("#templatelist");
                    templateUI.html("");
                    for (let ind in listdata)
                    {
                        let item = listdata[ind];
                        let popupContent = `<div class="col-md-2">
                                      <div class="gallery-card">
                                        <div class="gallery-card-body">
                                          <label class="block-check">
                                         <img src="${item['img']}" class="img-responsive" />
                                         <input type="checkbox" class="templatecheckbox" data-file="${item['file']}">
                                          <span class="checkmark"></span>
                                          </label>
                                           <div class="mycard-footer">
                                            <a href="#" class="card-link">${item['name']}</a>
                                            </div>
                                        </div>
                                      </div>
                                    </div>`;
                        templateUI.append(popupContent);
                    }

                     $(".templatecheckbox").change(function() {
                        if(this.checked) {
                            $(".templatecheckbox").prop('checked', false);
                            $(this).prop('checked', true);
                        }
                    });

                })
                .catch(function(error) {
                  console.log('Looks like there was a problem: when fetching template list ', error);
                });
}

function requestCreateProject() //Too lazy to rewrite it as a promise
{
            let template = "templates/default.tex";
            if(status !== "idle"){
                return;
            }
            let newpid = $("#projectnameinputfield").val();
            if(newpid === undefined || newpid === "")
            {
                newpid = "NewProject";
            }

            newpid = sanitizeString(newpid);
            let checkboxui = $(".templatecheckbox:checked");
            if(checkboxui.length === 0)
            {
                console.log("empty template");

            }
            else {
                template = checkboxui.attr("data-file");
            }

            status = "busy";
            NProgress.start();
            fetch(template).then(function(response) {
                  if (!response.ok) {
                    throw Error(response.statusText);
                  }
                  // Read the response as json.
                  return response.text();
                })
                .then(function(text) {
                   return networkClient.createProject(newpid, text)
                }).then(_ => {
                    return networkClient.listProject();
                }).then(data => {
                        popupProjectList(data);
                        status = "idle";
                        NProgress.done();
                }).catch(function(error) {
                  fatal_error('Looks like there was a problem when getting the template: ' + error);
                });

}

function renewToken()
{
    fetchToken().then(_ =>{
        setTimeout(renewToken, 300000);
    }).catch(err => {
        fatal_error("Error when renewing Token " +err);
    });
}

function fetchVersion(){

    fetch('/version.txt')
        .then(response => {
            if(response.status === 200){
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


fetchToken().then(_ =>{
    setTimeout(renewToken, 300000);
    $("#nametag").html(window.username);
    return buildNetworkClient();
}).then(nc=>{
    networkClient = nc;
    return networkClient.listProject();
}).then(data => {
    popupProjectList(data);
    status = "idle";
}).catch(err => {
    fatal_error(err);
});

fetchVersion();



