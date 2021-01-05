
export function initUser() {
    let userId = localStorage.getItem("userId")
    if(userId) {
        return userId;
    }

    var array = new Uint32Array(8);
    window.crypto.getRandomValues(array);
    let hexString = ""
    for (const item in array) {
        hexString += array[item].toString(16)
    }
    userId = hexString.substring(0,hexString.length-10)
    localStorage.setItem("userId", userId)
    return userId
}