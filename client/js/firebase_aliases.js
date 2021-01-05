"use strict";

var _aliasBrowsing = false;


var _firstBrowseWrongAlias = false;
function browseToAlias(alias, browseFn) {
    console.log("browse to alias...")
    console.log(alias)
    _db.collection("aliases").get().then((querySnapshot) => {
        var foundMatch = false;
        querySnapshot.forEach((doc) => {
            // console.log(`${doc.id} => ${doc.data()}`);

            if (doc.id === alias) {
                _aliasBrowsing = true;
                if (isNotSupported()) {
                    document.location = doc.data().url;
                } else {
                    browseFn(doc.data().url);
                    foundMatch = true;
                }
            }
        });
    });

}


function addAlias(alias, url, callback) {
    _db.collection("aliases").doc(alias).set({
        url: url
    })
        .then(function () {
            callback(true);

        })
        .catch(function (error) {
            callback(false);
            console.error("Error writing document: ", error);

        });
}

function isNotSupported() {
    var ua = navigator.userAgent;
    /* MSIE used to detect old browsers and Trident used to newer ones*/
    console.log(ua);
    var is_not_supported = ua.indexOf("MSIE ") > -1 || ua.indexOf("Trident/") > -1;

    return is_not_supported;
}
