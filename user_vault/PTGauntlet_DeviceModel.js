// Updated parameters for revised stacking:

// MPU dimensions and placement
mpu_w = 16;       // MPU width (x)
mpu_h = 26;       // MPU height (y)
mpu_depth = 7;    // MPU thickness (z)
// MPU: left overhang of 7 mm, so x: [-7, 9].
// Top of MPU is 2 mm below PCB top (PCB top at 48.3 mm → MPU top at 46.3 mm).
// Thus, MPU y-range: [46.3 - 26, 46.3] = [20.3, 46.3].

// PCB (Perfboard) dimensions and placement
pcb_w = 44.5;     // PCB width (x)
pcb_h = 48.3;     // PCB height (y)
pcb_thickness = 1.0;  // Adjusted PCB thickness for a total height of ~15 mm.
// PCB placed with its bottom-left at (0,0) in plan view.
// Z offset for PCB is above MPU: at z = mpu_depth (7 mm), so PCB spans z: 7 to 8.

// ESP32 board dimensions and placement
esp_w = 28;       // ESP32 board width (x)
esp_l = 53;       // ESP32 board length (y)
esp_thickness = 7;  // ESP32 block thickness (z)
 // Horizontal placement: Center of PCB is 22.25 mm; offset by 2.54 mm right gives ~24.79 mm.
 // So, ESP32 x-range: [24.79 - 14, 24.79 + 14] ≈ [10.79, 38.79].
 // Vertical placement: bottom at y = 4, top at y = 4 + 53 = 57.
 // Z: from PCB top (z = 8) to 8 + 7 = 15.

// USB-C port dimensions and placement (as an extension from the ESP32's Y-top face)
usb_w = 8;        // USB width (x)
usb_l = 2;        // USB length (y) extension from the top face of the ESP32
 // Centered on ESP32: center x ~24.79 mm → x-range: [20.79, 28.79].
 // Extends in Y from the ESP32's top at y = 57 to 57 + 2 = 59.
 // USB's z is identical to the ESP32: from 8 to 15.

// Modules
module mpu_block() {
  translate([-7, 20.3, 0])
    cube([mpu_w, mpu_h, mpu_depth]);
}

module pcb_block() {
  translate([0, 0, mpu_depth])
    cube([pcb_w, pcb_h, pcb_thickness]);
}

module esp32_block() {
  translate([10.79, 4, 7 + pcb_thickness])  // starting at z = 8
    cube([esp_w, esp_l, esp_thickness]);
}

module usb_block() {
  translate([20.79, 57, 7 + pcb_thickness])  // same z start as ESP32
    cube([usb_w, usb_l, esp_thickness]);
}

union() {
  mpu_block();
  pcb_block();
  esp32_block();
  usb_block();
}
