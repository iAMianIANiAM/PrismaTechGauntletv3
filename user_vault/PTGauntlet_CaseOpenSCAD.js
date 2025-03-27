//////////////////////
// PARAMETERS
//////////////////////

// Case dimensions
box_length      = 58;    // X dimension (mm)
box_width       = 55;    // Y dimension (mm) -- (Y=0 is the BACK edge, increases toward the front)
base_thickness  = 2;     // Base thickness (mm)
wall_thickness  = 1;     // Wall thickness (mm)
wall_height     = 18;    // Wall height (mm)
h_total         = base_thickness + wall_height; // Total height = 20 mm

// USB passthrough hole parameters (on left face, x=0)
// Coordinates measured from the BACK-LEFT corner of the case:
// The hole spans in Y from 27 mm to 39 mm and in Z from 12 mm to 18 mm.
usb_hole_y0 = 27;
usb_hole_y1 = 39;
usb_hole_z0 = 12;
usb_hole_z1 = 18;

// Shelf parameters (for Back and Right faces)
shelf_depth   = 5;       // Depth of shelf (mm)
shelf_height  = 4;       // Shelf thickness (mm)

// USB Tunnel parameters (left side)
// (These remain as before.)
cutout_width   = usb_hole_y1 - usb_hole_y0;  // 12 mm
cutout_height  = usb_hole_z1 - usb_hole_z0;  // 6 mm
cutout_bottom  = usb_hole_z0;                // 12 mm

tunnel_thickness = 15;   // How far the tunnel extends outward (mm)
tunnel_wall      = 2;    // Tunnel wall thickness (mm)
tunnel_outer_y   = cutout_width + 2*tunnel_wall; // 12 + 4 = 16 mm
tunnel_outer_z   = cutout_height + 2*tunnel_wall; // 6 + 4 = 10 mm
// Position the tunnel so its inner cavity aligns with the USB passthrough.
tunnel_y_start   = usb_hole_y0 - tunnel_wall;  // 27 - 2 = 25 mm
tunnel_z_start   = cutout_bottom - tunnel_wall;  // 12 - 2 = 10 mm

// Right outlet channel (for VGS wires)
outlet_width     = 16;    // Channel width (mm)
outlet_height    = 12;    // Channel height (mm)
outlet_y_start   = (box_width - outlet_width) / 2;  // (55 - 16)/2 = 19.5 mm (approx 19.5 mm)
outlet_z_start   = base_thickness;                // = 2 mm (starts at top of base)
outlet_extension = 10;    // Extra extension (mm)

// Right shelf notch (to clear the outlet channel)
notch_width   = 15;       // Must match outlet_width? (You can adjust if needed)
notch_y_start = (box_width - notch_width) / 2;       // (55 - 15)/2 = 20 mm
notch_z_start = base_thickness;                      // = 2 mm
notch_height  = 10;      // (mm)

// 3/4 Cylinder corner supports (for case mounting)
cyl_d         = 5;     // Full cylinder diameter (mm)
screw_hole_d  = 3;     // 3 mm screw hole diameter

// USB Fill Block parameters (new):
// Fill in the negative space beneath the exterior USB passthrough channel.
// Block dimensions: 15 mm (X) by 16 mm (Y) by 10 mm (Z).
// We want its right face flush with x=0, its Y range centered on the USB passthrough,
// and its Z range from 0 to 10.
usb_block_dim_x = 15;
usb_block_dim_y = 16;
usb_block_dim_z = 10;
  // To center in Y: the USB channel’s center is (usb_hole_y0+usb_hole_y1)/2 = (27+39)/2 = 33.
  // So the block should extend from 33 - (16/2) = 33 - 8 = 25 to 33 + 8 = 41 in Y.

//////////////////////
// CASE MODULES
//////////////////////

module shell() {
  difference() {
    // Full solid block (base + walls)
    cube([box_length, box_width, base_thickness + wall_height]);
    // Hollow interior (leaving walls of thickness wall_thickness)
    translate([wall_thickness, wall_thickness, base_thickness])
      cube([box_length - 2*wall_thickness, box_width - 2*wall_thickness, wall_height]);
    // Subtract USB passthrough hole from left face (x=0)
    translate([0, usb_hole_y0, usb_hole_z0])
      cube([wall_thickness, usb_hole_y1 - usb_hole_y0, usb_hole_z1 - usb_hole_z0]);
  }
}

module shelves() {
  union() {
    // Back shelf: along back face at y = box_width - wall_thickness - shelf_depth.
    translate([wall_thickness, box_width - wall_thickness - shelf_depth, base_thickness])
      cube([box_length - 2*wall_thickness, shelf_depth, shelf_height]);
    // Right shelf: along right face at x = box_length - wall_thickness - shelf_depth.
    translate([(box_length - wall_thickness) - shelf_depth, wall_thickness, base_thickness])
      cube([shelf_depth, box_width - 2*wall_thickness, shelf_height]);
  }
}

module usb_tunnel() {
  difference() {
    // Outer tunnel block on the left (x from -tunnel_thickness to 0)
    translate([-tunnel_thickness, tunnel_y_start, tunnel_z_start])
      cube([tunnel_thickness, tunnel_outer_y, tunnel_outer_z]);
    // Subtract inner cavity (USB opening) so the tunnel forms a collar.
    translate([-tunnel_thickness, usb_hole_y0, cutout_bottom])
      cube([tunnel_thickness, cutout_width, cutout_height]);
  }
}

module usb_channel() {
  translate([-tunnel_thickness, usb_hole_y0, cutout_bottom])
    cube([tunnel_thickness + wall_thickness, cutout_width, cutout_height]);
}

module right_outlet_channel() {
  // Subtract the channel from z = base_thickness+2 upward (leaving a 2mm base extension)
  translate([box_length - wall_thickness, outlet_y_start, base_thickness + 2])
    cube([wall_thickness + outlet_extension, outlet_width, outlet_height - 2]);
}

module right_shelf_notch() {
  translate([box_length - wall_thickness - shelf_depth, notch_y_start, notch_z_start])
    cube([shelf_depth, notch_width, notch_height]);
}

module right_base_extension() {
  translate([box_length, outlet_y_start, 0])
    cube([outlet_extension, outlet_width, base_thickness + outlet_height]);
}

// Helper: sector() for corner supports.
module sector(r, start_angle, end_angle, height) {
  linear_extrude(height = height)
    polygon(points = concat([[0,0]], [ for(a = [start_angle : 5 : end_angle]) [ r * cos(a), r * sin(a)] ]));
}

// Helper: three_quarter_cylinder() for corner supports.
module three_quarter_cylinder(angle_offset, h) {
  difference() {
    cylinder(d = cyl_d, h = h, $fn = 64);
    sector(cyl_d/2, angle_offset, angle_offset + 90, h);
  }
}

// Module: usb_fill_block()
// Fills in the negative space below the exterior USB passthrough channel.
// The block spans in X from -usb_block_dim_x to 0,
// in Y from 25 to 41 (16 mm wide), and in Z from 0 to usb_block_dim_z (10 mm tall).
module usb_fill_block() {
  translate([-usb_block_dim_x, 25, 0])
    cube([usb_block_dim_x, usb_block_dim_y, usb_block_dim_z]);
}

//////////////////////
// FINAL CASE ASSEMBLY
//////////////////////

module case_with_screw_holes() {
  difference() {
    union() {
      // Main body: shell, shelves, USB tunnel, right base extension.
      union() {
        shell();
        shelves();
        usb_tunnel();
        right_base_extension();
      }
      // Add four 3/4 cylinder corner supports.
      translate([0, 0, 0])
        three_quarter_cylinder(0, h_total);
      translate([box_length, 0, 0])
        three_quarter_cylinder(90, h_total);
      translate([box_length, box_width, 0])
        three_quarter_cylinder(180, h_total);
      translate([0, box_width, 0])
        three_quarter_cylinder(270, h_total);
      // Add the USB fill block to fill in the negative space below the exterior USB passthrough.
      usb_fill_block();
    }
    // Subtract the USB channel (to keep that passage open).
    usb_channel();
    // Subtract the modified right outlet channel (creating the VGS passthrough).
    right_outlet_channel();
    // Subtract the notch from the right shelf.
    right_shelf_notch();
    // Subtract 3 mm screw holes through the centers of the corner supports.
    translate([0, 0, 0])
      cylinder(d = screw_hole_d, h = h_total + 0.1, $fn = 32);
    translate([box_length, 0, 0])
      cylinder(d = screw_hole_d, h = h_total + 0.1, $fn = 32);
    translate([box_length, box_width, 0])
      cylinder(d = screw_hole_d, h = h_total + 0.1, $fn = 32);
    translate([0, box_width, 0])
      cylinder(d = screw_hole_d, h = h_total + 0.1, $fn = 32);
  }
}

//////////////////////
// RENDER
//////////////////////

case_with_screw_holes();
