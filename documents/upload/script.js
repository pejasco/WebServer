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
        let fileName = file.name; //getting selected file name
        if(fileName.length >= 12){ //if filename length is greater or equal to 12 the split that name and add ...
            let splitName = fileName.split('.');
            fileName = splitName[0].substring(0, 12) + "... ." + splitName[1];

        }
        uploadFile(fileName);//calling uploadFile with passing file name as an argument
    }
}

function uploadFile(name){
    let xhr = new XMLHttpRequest(); //creating new xml obj (AJAX)
    xhr.open("POST", "http://localhost:9000/upload"); //sending post request to the specified URL/File
    xhr.upload.addEventListener("progress", (e) =>{
        let fileLoaded = Math.floor((e.loaded/e.total) * 100);
        // let fileTotal = Math.floor(e.total / 1000);
        let fileSize;
        //if file size is less than 1024 then add only KB else convert size into KB to MB
        if (e.total < 1024 * 1024) {
            fileSize = (e.total / 1024).toFixed(2) + " KB";
        } else {
            fileSize = (e.total / (1024 * 1024)).toFixed(2) + " MB";
        }
        let progressHTML = `<li class="row">
                                <i class="fas fa-file-alt"></i>
                                <div class="content">
                                <div class="details">
                                    <span class="name">${name} · Uploading</span>
                                    <span class="percent">${fileLoaded}</span>
                                </div>
                                <div class="progress-bar">
                                    <div class="progress" style="width: ${fileLoaded}%"></div>
                                </div>
                                </div>
                            </li>`;
        // uploadedHTML.innerHTML = ""; yoyo
        uploadedArea.classList.add("onprogress");
        progressArea.innerHTML = progressHTML;
        if(e.loaded == e.total){
            progressArea.innerHTML = "";
            let uploadedHTML = `<li class="row">
                                    <div class="content">
                                        <i class="fas fa-file-alt"></i>
                                        <div class="details">
                                            <span class="name">${name} · Uploaded</span>
                                            <span class="size">${fileSize}</span>
                                        </div>
                                    </div>
                                            <button class="delete-btn" data-filename="${name}" title="Delete">Del</button>
                                </li>`;
            
            // uploadedArea.innerHTML = uploadedHTML; yoyo
            uploadedArea.classList.remove("onprogress");
            uploadedArea.insertAdjacentHTML("afterbegin", uploadedHTML);
        }
    });
    let formData = new FormData(form); //formData is an object to an object to easily send from data
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