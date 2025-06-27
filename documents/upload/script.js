const form = document.querySelector("form"),
fileInput = form.querySelector(".file-input"),
progressArea = document.querySelector(".progress-area"),
uploadedArea = document.querySelector(".uploaded-area");

form.addEventListener("click", ()=>{
    fileInput.click();
});

fileInput.onchange = ({target}) =>{
    // console.log(target.files);
    let file = target.files[0]; //getting file and [0] this means if user has selected multiples files the
    if (file){ //if file is selected
        let shortFileName = file.name; //getting selected file name
        let fullFileName = file.name;
        if(fullFileName.length >= 12){ //if filename length is greater or equal to 12 the split that name and add ...
            let splitName = shortFileName.split('.');
            shortFileName = splitName[0].substring(0, 12) + "... ." + splitName[1];

        }
        uploadFile(fullFileName, shortFileName);//calling uploadFile with passing file name as an argument
    }
}

function uploadFile(fullName, shortName){
    let xhr = new XMLHttpRequest();
    xhr.open("POST", "http://localhost:9000/storage");
    HasError = 0;
    const errorElements = uploadedArea.querySelectorAll('.error');
    errorElements.forEach(element => element.remove());

    // Adding error handling
    xhr.onreadystatechange = function() {
        if (xhr.readyState === XMLHttpRequest.DONE) {
            if (xhr.status === 413) {
                // Handle 413 File Too Large error - show inline error message only
                progressArea.innerHTML = "";
                uploadedArea.classList.remove("onprogress");
                
                let errorHTML = `<li class="row error">
                                    <i class="fas fa-exclamation-triangle" style="color: red;"></i>
                                    <div class="content">
                                        <div class="details">
                                            <span class="name" style="color: red;">${shortName} · File Too Large!</span>
                                        </div>
                                    </div>
                                </li>`;
                uploadedArea.insertAdjacentHTML("afterbegin", errorHTML);
                HasError = 1;
                return; // Exit function - no progress tracking needed
                
            } else if (xhr.status >= 400 && xhr.status != 413) {
                progressArea.innerHTML = "";
                uploadedArea.classList.remove("onprogress");
                
                // Replace the entire page with server's error page
                document.body.innerHTML = xhr.responseText;
                return ;
            }
        }
    };

    // Progress event listener - this won't run if we return early from error handling
    xhr.upload.addEventListener("progress", (e) =>{
        if (HasError == 1)
            return;
        let fileLoaded = Math.floor((e.loaded/e.total) * 100);
        let fileSize;
        
        if (e.total < 1024 * 1024) {
            fileSize = (e.total / 1024).toFixed(2) + " KB";
        } else {
            fileSize = (e.total / (1024 * 1024)).toFixed(2) + " MB";
        }
        
        let progressHTML = `<li class="row">
                                <i class="fas fa-file-alt"></i>
                                <div class="content">
                                <div class="details">
                                    <span class="name">${shortName} · Uploading</span>
                                    <span class="percent">${fileLoaded}%</span>
                                </div>
                                <div class="progress-bar">
                                    <div class="progress" style="width: ${fileLoaded}%"></div>
                                </div>
                                </div>
                            </li>`;
        
        uploadedArea.classList.add("onprogress");
        progressArea.innerHTML = progressHTML;
        
        // When upload is complete, show success message with delete button
        setTimeout(() => {
            if (HasError != 1) {
                if(e.loaded == e.total){
                    progressArea.innerHTML = "";
                    let uploadedHTML = `<li class="row">
                                            <div class="content">
                                                <i class="fas fa-file-alt"></i>
                                                <div class="details">
                                                    <span class="name">${shortName} · Uploaded</span>
                                                    <span class="size">${fileSize}</span>
                                                </div>
                                            </div>
                                            <button class="delete-btn" data-filename="${fullName}" title="Delete">Del</button>
                                        </li>`;

                    uploadedArea.classList.remove("onprogress");
                    uploadedArea.insertAdjacentHTML("afterbegin", uploadedHTML);
                }
            }
        }, 500); // small delay to avoid displaying delete button if file hasn't been uploaded correctly
    });
    
    let formData = new FormData(form);
    xhr.send(formData);
}
document.addEventListener("DOMContentLoaded", function() {
    const header = document.getElementById("flashing-header");
    if (!header) return;
    const texts = ["WEBSERVVV FILE UPLOADER", "Rosé the BESTTTT!"];
    let idx = 0;
    setInterval(() => {
        idx = 1 - idx;
        header.textContent = texts[idx];
    }, 1500); // matches your 1s flash animation
});

uploadedArea.addEventListener("click", function(e) {
    if (e.target.classList.contains("delete-btn")) {
        const btn = e.target;
        const filename = btn.getAttribute("data-filename");
        
        fetch(`/storage/${filename}`, { method: "DELETE" })
            .then(res => {
                if (res.ok) {
                    btn.closest(".row").remove();
                    console.log("Delete success");
                    fileInput.value = '';
                    alert("Delete success!");
                } else {
                    throw new Error("Failed to delete file");
                }
            })
            .catch(error => {
                alert("Failed to delete file: " + error.message);
            });
    }
});