$("#createprojbtn").click(function () {
    $('#createProjectModal').modal({
        keyboard: true
    });
});

$(".renamebtn").click(function () {
    let pid = $(this).data("pid");
    let userProvideName = prompt("Rename your project", "New name for your project");
    if (!userProvideName)
        return;
    $("#rename_project_id_control").val(pid);
    $("#rename_project_name_control").val(userProvideName);
    $("#rename_project_form").submit();
});

$(".removebtn").click(function () {
    let pid = $(this).data("pid");
    if (confirm("Would you like to delete this project")) {
        $("#delete_project_id_control").val(pid);
        $("#delete_project_form").submit();
    }
});

$(".sharebtn").click(function () {
    const pid = $(this).data("pid");
    $("#share-link-input").val(window.location.protocol + '//' + window.location.hostname + '/u/' + pid + '/');
    $('#shareProjectModal').modal({
        keyboard: true
    });
});

$("#copylinkbtn").click(function () {
    const copyText = document.getElementById("share-link-input");

    /* Select the text field */
    copyText.select();
    copyText.setSelectionRange(0, 99999); /*For mobile devices*/

    /* Copy the text inside the text field */
    document.execCommand("copy");
    alert('The link has been copied to your clipboard');
});