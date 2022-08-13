import * as os from "os";

function Uint8ArrayToString(fileData){
    var dataString = "";
    for (var i = 0; i < fileData.length; i++) {
      dataString += String.fromCharCode(fileData[i]);
    }
   
    return dataString
}  

var buf = new Uint8Array(64)
os.read(0, buf.buffer, 0, 64)

console.log(Uint8ArrayToString(buf), "World")