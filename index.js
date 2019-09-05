const binding = require('./build/Release/mbus_wm_gb.node');
const obj1 = new binding.Mbus();

obj1.device = "COM4";
obj1.baudrate = 2400;
obj1.databits = 8;
obj1.stopbits = 1;
obj1.parity = 'e';
console.log(obj1.device + ", " + obj1.baudrate  + ", " + obj1.databits  + ", " + obj1.stopbits  + ", " + obj1.parity);

var addr = Buffer.from([0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA]);
var payload = Buffer.from([0x1F, 0x90, 0x00]);

obj1.metertype = 0x10;
obj1.addr      = addr;
console.log("addr: " + obj1.addr.toString('hex'));

if (0 != obj1.connect()) {
    console.log("device connect failure."); 
} 

if (0 == obj1.send(0x01, payload)) {
    console.log("device send success."); 
} else {
    console.log("device send failure."); 
}

var packet = obj1.recv();
if (0 == packet.code) {
    console.log("device recv success."); 
    console.log("Meter Type: " + packet.metertype); 
    console.log("Address: " + packet.addr.toString('hex')); 
    console.log("Control: " + packet.ctrl); 
    console.log("Payload lenght: " + packet.payload.length); 
    console.log("Payload: " + packet.payload.toString('hex')); 
} else {
    console.log("device recv failure, Code " + packet.code); 
}

if (0 != obj1.disconnect()) {
    console.log("device disconnect failure."); 
} 