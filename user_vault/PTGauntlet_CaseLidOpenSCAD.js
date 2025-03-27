//////////////////////
// GLOBAL PARAMETERS
//////////////////////

// Lid footprint (matches case)
box_length    = 58;   // mm (X dimension)
box_width     = 55;   // mm (Y dimension)
lid_thickness = 3;    // Lid plate thickness (mm)

// Boss (corner ring) parameters
boss_d        = 5;    // Boss outer diameter (mm)
boss_height   = 3;    // Boss height (mm) – built into lid so top is flush
screw_hole_d  = 3;    // Screw hole diameter (mm)

// Ventilation (hex grid) parameters
vent_margin   = 5;                      // Margin from lid edges (mm)
vent_spacing  = 5;                      // Horizontal spacing between vents (mm)
vent_v_spacing = vent_spacing * 0.866;    // Vertical spacing for hex grid (approx)
n_cols = floor((box_length - 2 * vent_margin) / vent_spacing) + 1;
n_rows = floor((box_width - 2 * vent_margin) / vent_v_spacing) + 1;

//////////////////////
// HELPER MODULES
//////////////////////

// Hexagon: creates a 2D hexagon with side length 's'
// (For s = 2.31, the flat-to-flat distance is approximately 4 mm.)
module hexagon(s) {
  pts = [
    [ s, 0],
    [ s/2, s * 0.866],
    [-s/2, s * 0.866],
    [-s, 0],
    [-s/2, -s * 0.866],
    [ s/2, -s * 0.866]
  ];
  polygon(points = pts);
}

//////////////////////
// LID MODULES
//////////////////////

// lid_plate: creates the 2 mm–thick lid plate and subtracts the hexagon vent pattern.
module lid_plate() {
  difference() {
    // Base lid plate
    cube([box_length, box_width, lid_thickness]);
    // Subtract ventilation holes in a hexagonal grid
    for (row = [0 : n_rows - 1]) {
      for (col = [0 : n_cols - 1]) {
         translate([ vent_margin + col * vent_spacing + (row % 2) * (vent_spacing/2),
                     vent_margin + row * vent_v_spacing, 0 ])
           linear_extrude(height = lid_thickness)
             hexagon(2.31);
      }
    }
  }
}

// boss_ring: creates a boss ring—a 5 mm–diameter, 1 mm–high cylinder with a 3 mm hole subtracted.
module boss_ring() {
  difference() {
    cylinder(d = boss_d, h = boss_height, $fn = 64);
    cylinder(d = screw_hole_d, h = boss_height + 0.1, $fn = 32);
  }
}

// boss_union: places a boss ring at each of the four lid corners so that the top surfaces are flush with the lid.
module boss_union() {
  union() {
    translate([0, 0, lid_thickness - boss_height]) boss_ring();
    translate([box_length, 0, lid_thickness - boss_height]) boss_ring();
    translate([box_length, box_width, lid_thickness - boss_height]) boss_ring();
    translate([0, box_width, lid_thickness - boss_height]) boss_ring();
  }
}

// Final lid: union of the vented lid plate and the boss rings.
module lid() {
  union() {
    lid_plate();
    boss_union();
  }
}

//////////////////////
// FINAL ASSEMBLY
//////////////////////

lid();
